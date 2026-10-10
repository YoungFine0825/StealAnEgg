#include "SAEPlayerState.h"

#include "AbilitySystem/SAEAbilitySystemComponent.h"
#include "AbilitySystem/SAEGameplayTags.h"
#include "AbilitySystem/SAEPlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/SAEQuickbarComponent.h"
#include "Inventory/SAEHandComponent.h"
#include "Inventory/SAEStolenEggComponent.h"

ASAEPlayerState::ASAEPlayerState()
{
	QuickbarComponent = CreateDefaultSubobject<USAEQuickbarComponent>(TEXT("QuickbarComponent"));

	HandComponent = CreateDefaultSubobject<USAEHandComponent>(TEXT("HandComponent"));

	StolenEggComponent = CreateDefaultSubobject<USAEStolenEggComponent>(TEXT("StolenEggComponent"));

	AbilitySystemComponent = CreateDefaultSubobject<USAEAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PlayerAttributeSet = CreateDefaultSubobject<USAEPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
}

void ASAEPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASAEPlayerState,JoinState);
	DOREPLIFETIME(ASAEPlayerState,AssignedPlotIndex);
}

void ASAEPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	//PlayerState率先初始化
	AbilitySystemComponent->InitAbilityActorInfo(this,nullptr);

	//服务端授予一次默认Ability
	//授予Ability只要求AbilitySystemComponent存在，因此InitAbilityActorInfo的InAvatarActor传nullptr也是可以的。
	//当调用InitAbilitySystemAvatar绑定Avatar,GAS会更新ActorInfo并通知Ability。
	GrantDefaultAbilities();
}

//后续Character将调用此方法绑定Avatar
void ASAEPlayerState::InitAbilitySystemAvatar(AActor* AbilitySystemAvatar)
{
	AbilitySystemComponent->InitAbilityActorInfo(this,AbilitySystemAvatar);
}

//授予默认Ability
void ASAEPlayerState::GrantDefaultAbilities()
{
	if(!HasAuthority() || !AbilitySystemComponent)
	{
		return;
	}
	//默认授予玩家GA_Interact
	if(InteractAbilityClass && !AbilitySystemComponent->FindAbilitySpecFromClass(InteractAbilityClass))
	{
		FGameplayAbilitySpec InteractSpec(
			InteractAbilityClass,
			1,
			INDEX_NONE,
			this);
		InteractSpec.GetDynamicSpecSourceTags().AddTag(SAEGameplayTags::Input_Interact);
		InteractAbilityHandle = AbilitySystemComponent->GiveAbility(InteractSpec);
	}
	//默认授予玩家GA_Pickup
	if(PickupAbilityClass && !AbilitySystemComponent->FindAbilitySpecFromClass(PickupAbilityClass))
	{
		FGameplayAbilitySpec PickupSpec(
			PickupAbilityClass,
			1,
			INDEX_NONE,
			this);
		PickupAbilityHandle = AbilitySystemComponent->GiveAbility(PickupSpec);
	}
	for(const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if(!AbilityClass || AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass))
		{
			continue;
		}
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass,1));
	}
}


void ASAEPlayerState::SetJoinState(ESAEPlayerJoinState NewState)
{
	if(!HasAuthority() || JoinState == NewState)
	{
		return;
	}
	JoinState = NewState;
	JoinStateChangedDelegate.Broadcast(NewState);
}

void ASAEPlayerState::OnRep_JoinState()
{
	JoinStateChangedDelegate.Broadcast(JoinState);
}


void ASAEPlayerState::SetAssignedPlotINdex(int32 NewPlotIndex)
{
	if(!HasAuthority() || AssignedPlotIndex == NewPlotIndex)
	{
		return;
	}
	AssignedPlotIndex = NewPlotIndex;
	PlotChangedDelegate.Broadcast(NewPlotIndex);
}

void ASAEPlayerState::OnRep_AssignedPlotIndex()
{
	PlotChangedDelegate.Broadcast(AssignedPlotIndex);
}

USAEQuickbarComponent* ASAEPlayerState::GetQuickbarComponent() const
{
	return QuickbarComponent;
}

USAEHandComponent* ASAEPlayerState::GetHandComponent() const
{
	return HandComponent;
}

USAEStolenEggComponent* ASAEPlayerState::GetStolenEggComponent() const
{
	return StolenEggComponent;
}


UAbilitySystemComponent* ASAEPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
