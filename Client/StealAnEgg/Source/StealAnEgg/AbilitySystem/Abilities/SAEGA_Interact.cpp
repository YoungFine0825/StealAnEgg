#include "SAEGA_Interact.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/SAEGameplayTags.h"
#include "AbilitySystem/Tasks/SAEAbilityTask_WaitForInteractableTargets.h"
#include "Components/ActorComponent.h"
#include "Interaction/SAEInteractableTarget.h"
#include "Interaction/SAEInteractionMessages.h"
#include "GameplayMessageRuntime/Public/GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SAEGA_Interact)

USAEGA_Interact::USAEGA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy =  EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void USAEGA_Interact::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	//在本地自动启动
	if(ActorInfo && ActorInfo->IsLocallyControlled() && ActorInfo->AbilitySystemComponent.IsValid() && !Spec.IsActive())
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle,false);
	}
}

void USAEGA_Interact::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(!ActorInfo || !ActorInfo->IsLocallyControlled())
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,false,true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if(!ASC)
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,false,true);
		return;
	}

	AbilityEndedDelegateHandle = ASC->OnAbilityEnded.AddUObject(this,&ThisClass::HandleAbilityEnded);
	ScanTasks = USAEAbilityTask_WaitForInteractableTargets::WaitForTargets(
		this,
		ScanRange,
		ScanRate,
		TraceRadius,
		TraceChannel);
	ScanTasks->OnOptionsChanged.AddDynamic(this,&ThisClass::HandleOptionsChanged);
	ScanTasks->ReadyForActivation();
}

void USAEGA_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	 const FGameplayAbilityActivationInfo ActivationInfo,
	  bool bReplicateEndAbility,
	  bool bWasCancelled)
{
	if(UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
	}

	AbilityEndedDelegateHandle.Reset();

	if(ScanTasks)
	{
		ScanTasks->EndTask();
		ScanTasks = nullptr;
	}

	CurrentOptions.Reset();
	SelectedOptionIndex = INDEX_NONE;
	ActiveInteractionAbilityHandle = FGameplayAbilitySpecHandle();
	BroadcastInteractionOptionsChanged(CurrentOptions,SelectedOptionIndex);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USAEGA_Interact::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	TriggerInteraction();
}

void USAEGA_Interact::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ReleaseInteraction();
}

void USAEGA_Interact::SetSelectedOptionIndex(int32 NewIndex)
{
	SelectedOptionIndex = CurrentOptions.IsValidIndex(NewIndex) ? NewIndex : INDEX_NONE;
	BroadcastInteractionOptionsChanged(CurrentOptions,SelectedOptionIndex);
}


void USAEGA_Interact::HandleOptionsChanged(const TArray<FSAEInteractionOption>& Options)
{
	UObject* PreviousTarget = nullptr;
	FGameplayTag PreviousTag;

	if(CurrentOptions.IsValidIndex(SelectedOptionIndex))
	{
		PreviousTarget = CurrentOptions[SelectedOptionIndex].Target.GetObject();
		PreviousTag = CurrentOptions[SelectedOptionIndex].InteractionTag;
	}

	CurrentOptions = Options;
	SelectedOptionIndex = INDEX_NONE;

	for(int32 i = 0; i < CurrentOptions.Num(); ++i)
	{
		if(CurrentOptions[i].Target.GetObject() == PreviousTarget && CurrentOptions[i].InteractionTag == PreviousTag)
		{
			SelectedOptionIndex = i;
			break;
		}
	}

	if(SelectedOptionIndex == INDEX_NONE && !CurrentOptions.IsEmpty())
	{
		SelectedOptionIndex = 0;//select first
	}

	BroadcastInteractionOptionsChanged(CurrentOptions,SelectedOptionIndex);
}

void USAEGA_Interact::BroadcastInteractionOptionsChanged(const TArray<FSAEInteractionOption>& Options, int32 SelectedIndex)
{
	OnInteractionOptionsChanged(CurrentOptions,SelectedOptionIndex);

	FSAEInteractionOptionsChangedMessage Message(CurrentOptions,SelectedIndex);
	if(UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		SAEGameplayTags::Event_Interaction_UI_OptionsChanged,
		Message
		);
	}
}


bool USAEGA_Interact::TriggerInteraction()
{
	if(ActiveInteractionAbilityHandle.IsValid() || !CurrentOptions.IsValidIndex(SelectedOptionIndex))
	{
		return false;
	}

	const FSAEInteractionOption& Option = CurrentOptions[SelectedOptionIndex];
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayAbilitySpec* Spec = ASC && Option.AbilityClass ? ASC->FindAbilitySpecFromClass(Option.AbilityClass) : nullptr;
	UObject* TargetObject = Option.Target.GetObject();
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if(!TargetActor)
	{
		if(const UActorComponent* Component = Cast<UActorComponent>(TargetObject))
		{
			TargetActor = Component->GetOwner();
		}
	}

	if(!Spec || !IsValid(TargetActor))
	{
		OnInteractionActivationFailed(Option);
		return false;
	}

	FGameplayEventData Payload;
	Payload.EventTag = SAEGameplayTags::Event_Interaction_Activate;
	Payload.Instigator = GetAvatarActorFromActorInfo();
	Payload.Target = TargetActor;

	if(Option.InteractionTag.IsValid())
	{
		Payload.TargetTags.AddTag(Option.InteractionTag);
	}

	if(Option.Target.GetInterface())
	{
		Option.Target->CustomizeInteractionEventData(Payload.EventTag,Payload);
	}

	if(!Payload.Target.Get())
	{
		OnInteractionActivationFailed(Option);
		return false;
	}

	Spec->InputPressed = true;
	ASC->AbilitySpecInputPressed(*Spec);
	ActiveInteractionAbilityHandle = Spec->Handle;

	FGameplayAbilityActorInfo PickupActorInfo;
	PickupActorInfo.InitFromActor(ASC->GetOwnerActor(),ASC->GetAvatarActor(),ASC);

	//触发Ability
	const bool bActived = ASC->TriggerAbilityFromGameplayEvent(
		Spec->Handle,
		&PickupActorInfo,
		Payload.EventTag,
		&Payload,
		*ASC);

	if(!bActived)
	{
		Spec->InputPressed = false;
		ActiveInteractionAbilityHandle = FGameplayAbilitySpecHandle();
		OnInteractionActivationFailed(Option);
		return false;
	}
	return true;
}

void USAEGA_Interact::ReleaseInteraction()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayAbilitySpec* Spec = ASC ? ASC->FindAbilitySpecFromHandle(ActiveInteractionAbilityHandle) : nullptr;
	if(Spec)
	{
		ASC->AbilitySpecInputReleased(*Spec);
	}
}

void USAEGA_Interact::HandleAbilityEnded(const FAbilityEndedData& EndedData)
{
	if(EndedData.AbilitySpecHandle == ActiveInteractionAbilityHandle)
	{
		ActiveInteractionAbilityHandle = FGameplayAbilitySpecHandle();
	}
}
