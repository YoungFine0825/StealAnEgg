#include "SAEGA_Pickup.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/SAEGameplayTags.h"
#include "Interaction/SAEInteractableTarget.h"
#include "Interaction/SAEPickupable.h"
#include "Player/SAEPlayerCharacter.h"
#include "GameplayMessageRuntime/Public/GameFramework/GameplayMessageSubsystem.h"
#include "Interaction/SAEInteractionMessages.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SAEGA_Pickup)

USAEGA_Pickup::USAEGA_Pickup()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void USAEGA_Pickup::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bPickupStarted = false;
	bDurationFinished = false;
	bSuppressCancelEvent = false;
	PickupTargetActor = nullptr;
	PickupTarget.SetObject(nullptr);
	PickupTarget.SetInterface(nullptr);
	CachedPickupInfo = FSAEPickupInfo();

	FText FailureReason;
	if(!ResolveAndValidateTarget(TriggerEventData,FailureReason))
	{
		FailAndEnd(FText::AsLocalizable_Advanced_LocText(TEXT("Pickup"),TEXT("AbilityCommitFailed"),*FailureReason.ToString()));
		return;
	}

	if(!CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		FailAndEnd(NSLOCTEXT("Pickup","AbilityCommitFailed","当前无法执行拾取"));
		return;
	}

	float Duration = 0.0f;
	if(const ISAEInteractableTarget* Interactable = Cast<ISAEInteractableTarget>(PickupTargetActor))
	{
		Duration = FMath::Max(0.0f,Interactable->GetInteractionDuration());
	}

	bPickupStarted = true;
	if(IsLocallyControlledAbility())
	{
		OnPickupStarted(PickupTargetActor,CachedPickupInfo,Duration);
	}

	WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this,true);
	WaitInputReleaseTask->OnRelease.AddDynamic(this,&ThisClass::HandleInputRelease);
	WaitInputReleaseTask->ReadyForActivation();

	if(Duration <= 0.0f)
	{
		HandleDurationFinished();
		return;
	}

	WaitDurationTask = UAbilityTask_WaitDelay::WaitDelay(this,Duration);
	WaitDurationTask->OnFinish.AddDynamic(this,&ThisClass::HandleDurationFinished);
	WaitDurationTask->ReadyForActivation();

	FSAEPickupHoldMessage HoldEvent;
	HoldEvent.Duration = Duration;
	HoldEvent.PickupInfo = CachedPickupInfo;
	HoldEvent.bIsHolding = true;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		SAEGameplayTags::Event_Interaction_UI_Hold,
		HoldEvent
		);
}


bool USAEGA_Pickup::ResolveAndValidateTarget(const FGameplayEventData* TriggerEvent, FText& OutFailureReason)
{
	const AActor* EventTarget = TriggerEvent ? TriggerEvent->Target.Get() : nullptr;
	if(!EventTarget)
	{
		OutFailureReason = NSLOCTEXT("Pickup", "InvalidTarget","没有有效的拾取目标");
		return false;
	}
	PickupTargetActor = const_cast<AActor*>(EventTarget);
	if(!IsValid(PickupTargetActor))
	{
		OutFailureReason = NSLOCTEXT("Pickup", "InvalidTarget","没有有效的拾取目标");
		return false;
	}
	PickupTarget = USAEPickupableStatic::GetFirstPickupableFromActor(PickupTargetActor);
	if(!PickupTarget.GetInterface())
	{
		OutFailureReason = NSLOCTEXT("Pickup", "InvalidTarget","目标无法被拾取");
		return false;
	}

	ASAEPlayerCharacter* Character = GetSAECharacter();
	if(!Character)
	{
		OutFailureReason = NSLOCTEXT("Pickup", "InvalidCharacter","无效的角色");
		return false;
	}

	CachedPickupInfo = PickupTarget->GetPickupInfo();
	if(!CachedPickupInfo.IsValid())
	{
		OutFailureReason = NSLOCTEXT("Pickup", "InvalidPickupInfo","拾取物配置无效");
		return false;
	}
	return PickupTarget->CanBePickedUpBy(Character,OutFailureReason);
}

void USAEGA_Pickup::HandleDurationFinished()
{
	if(!IsActive())
	{
		return;
	}
	bDurationFinished = true;

	if(!PickupTarget.GetInterface() || !IsValid(PickupTargetActor))
	{
		FailAndEnd(NSLOCTEXT("Pickup","TargetLost","拾取目标已失效"));
		return;
	}

	if(!IsAuthorityAbility())//预测客户端不能抢先EndAbility，否则服务端实例可能被取消
	{
		if(IsLocallyControlledAbility())
		{
			OnPickupPredictedComplete(PickupTargetActor,CachedPickupInfo);
		}
		return;
	}

	ASAEPlayerCharacter* Character = GetSAECharacter();
	FText FailureReason;
	if(!Character
		|| !PickupTarget->CanBePickedUpBy(Character,FailureReason)//两个玩家可能同时在本地通过 CanBePickedUpBy()，服务器需要再次检查可否拾取
		|| !PickupTarget->CommitPickup(Character,FailureReason))//执行拾取
	{
		FailAndEnd(
			FailureReason.IsEmpty()?
			NSLOCTEXT("Pickup", "CommitFailed","拾取失败"):
			FailureReason
			);
		return;
	}

	if(IsLocallyControlledAbility())
	{
		OnPickupCommitted(PickupTargetActor,CachedPickupInfo);
	}

	OnPickupSucceeded(PickupTargetActor);

	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void USAEGA_Pickup::HandleInputRelease(float TimeHeld)
{
	if(!IsActive() || bDurationFinished)
	{
		return;
	}
	CancelAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true);
}

void USAEGA_Pickup::FailAndEnd(const FText& FailureReason)
{
	bSuppressCancelEvent = true;

	if(IsLocallyControlledAbility())
	{
		OnPickupFailed(PickupTargetActor,FailureReason);
	}

	//bReplicateEndAbility保持为true，让服务器出现失败也复制给客户端
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
}

void USAEGA_Pickup::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	if(bWasCancelled
		&& bPickupStarted
		&& !bSuppressCancelEvent
		&& IsLocallyControlledAbility()
		)
	{
		OnPickupCancelled(PickupTargetActor);
	}

	FSAEPickupHoldMessage HoldEvent;
	HoldEvent.Duration = 0;
	HoldEvent.PickupInfo = CachedPickupInfo;
	HoldEvent.bIsHolding = false;
	
	WaitDurationTask = nullptr;
	WaitInputReleaseTask = nullptr;
	bPickupStarted = false;
	bDurationFinished = false;
	bSuppressCancelEvent = false;
	PickupTargetActor = nullptr;
	PickupTarget.SetObject(nullptr);
	PickupTarget.SetInterface(nullptr);
	CachedPickupInfo = FSAEPickupInfo();

	if(UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(
			SAEGameplayTags::Event_Interaction_UI_Hold,
			HoldEvent
			);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

ASAEPlayerCharacter* USAEGA_Pickup::GetSAECharacter() const
{
	return Cast<ASAEPlayerCharacter>(GetAvatarActorFromActorInfo());
}

bool USAEGA_Pickup::IsLocallyControlledAbility() const
{
	return CurrentActorInfo && CurrentActorInfo->IsLocallyControlled();
}

bool USAEGA_Pickup::IsAuthorityAbility() const
{
	return CurrentActorInfo && CurrentActorInfo->IsNetAuthority();
}


