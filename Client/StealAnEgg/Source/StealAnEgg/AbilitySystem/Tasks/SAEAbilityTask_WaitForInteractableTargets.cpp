#include "SAEAbilityTask_WaitForInteractableTargets.h"

#include "Components/ActorComponent.h"
#include "GameFramework/Controller.h"
#include "Interaction/SAEInteractableTarget.h"
#include "Player/SAEPlayerCharacter.h"
#include "TimerManager.h"

/*
 * UE_INLINE_GENERATED_CPP_BY_NAME的目的：
 * 将UHT生成的代码与原.cpp文件放进同一编译单元
 * 减少额外的.gen.cpp编译单元的数量
 * 改善增量编译和链接效率
 * 让生成代码能利用当前.cpp的编译上下文
 * 
 */
#include "AbilitySystem/SAEGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SAEAbilityTask_WaitForInteractableTargets)

USAEAbilityTask_WaitForInteractableTargets* USAEAbilityTask_WaitForInteractableTargets::WaitForTargets(
  UGameplayAbility* OwningAbility,
  float ScanRange,
  float ScanRate,
  float TraceRadius,
  ECollisionChannel TraceChannel)
{
	USAEAbilityTask_WaitForInteractableTargets* Task = UAbilityTask::NewAbilityTask<USAEAbilityTask_WaitForInteractableTargets>(OwningAbility);
	Task->ScanRange = ScanRange;
	Task->ScanRate = ScanRate;
	Task->TraceChannel = TraceChannel;
	Task->TraceRadius = TraceRadius;
	return Task;
}

void USAEAbilityTask_WaitForInteractableTargets::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();

	PerformScan();

	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ScanTimerHandle,this,&ThisClass::PerformScan,ScanRate,true);
	}
}

void USAEAbilityTask_WaitForInteractableTargets::OnDestroy(bool bInOwnerFinished)
{
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ScanTimerHandle);
	}
	CachedOptions.Reset();
	Super::OnDestroy(bInOwnerFinished);
}

void USAEAbilityTask_WaitForInteractableTargets::AppendTarget(
	UObject* Object,
	TArray<TScriptInterface<ISAEInteractableTarget>>& OutTargets,
	TSet<UObject*>& SeenObjects) const
{
	if(!IsValid(Object)
		|| SeenObjects.Contains(Object)
		|| !Object->GetClass()->ImplementsInterface(USAEInteractableTarget::StaticClass())
		)
	{
		return;
	}

	ISAEInteractableTarget* Target = Cast<ISAEInteractableTarget>(Object);
	if(!Target)
	{
		return;
	}

	SeenObjects.Add(Object);
	TScriptInterface<ISAEInteractableTarget>& Added = OutTargets.AddDefaulted_GetRef();
	Added.SetObject(Object);
	Added.SetInterface(Target);
}

void USAEAbilityTask_WaitForInteractableTargets::PerformScan()
{
	ASAEPlayerCharacter* Character = Cast<ASAEPlayerCharacter>(GetAvatarActor());
	if(!Character
		|| !Character->IsLocallyControlled()
		|| !Character->GetController()
		|| !GetWorld()
		)
	{
		CachedOptions.Reset();
		OnOptionsChanged.Broadcast(CachedOptions);//为空时也同样广播出去
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	Character->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * ScanRange;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(SAEInteractioScane),false,Character);

	FHitResult Hit;
	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		ViewLocation,
		TraceEnd,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(TraceRadius),
		Params);

	TArray<TScriptInterface<ISAEInteractableTarget>> Targets;
	TSet<UObject*> SeenObjects;
	if(bHit)
	{
		AppendTarget(Hit.GetComponent(),Targets,SeenObjects);
		AppendTarget(Hit.GetActor(),Targets,SeenObjects);
	}

	TArray<FSAEInteractionOption> NewOptions;
	for(const TScriptInterface<ISAEInteractableTarget>& Target : Targets)
	{
		const ISAEInteractableTarget* Interface = Target.GetInterface();
		FText FailureReason;
		if(!Interface || !Interface->CanInteract(Character,FailureReason))
		{
			continue;
		}
		FSAEInteractionOptionBuilder Builder(Target,NewOptions);
		Interface->GatherInteractionOptions(Character,Builder);
	}

	if(!AreOptionsEqual(CachedOptions,NewOptions))
	{
		CachedOptions = MoveTemp(NewOptions);
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnOptionsChanged.Broadcast(CachedOptions);
		}
	}
}

bool USAEAbilityTask_WaitForInteractableTargets::AreOptionsEqual(const TArray<FSAEInteractionOption>& Left, const TArray<FSAEInteractionOption>& Right)
{
	if(Left.Num() != Right.Num())
	{
		return false;
	}
	for(int32 i = 0; i < Left.Num(); i++)
	{
		const FSAEInteractionOption& A = Left[i];
		const FSAEInteractionOption& B = Right[i];
		if(A != B)
		{
			return false;
		}
	}
	return true;
}
