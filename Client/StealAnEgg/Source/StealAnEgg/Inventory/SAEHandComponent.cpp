#include "SAEHandComponent.h"

#include "AbilitySystemComponent.h"
#include "SAEQuickbarComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/SAEPlayerState.h"
#include "Player/SAEPlayerCharacter.h"
#include "Player/SAEHeldItemPresentationComponent.h"
#include "AbilitySystem/SAEGameplayTags.h"

USAEHandComponent::USAEHandComponent()
{
	SetIsReplicatedByDefault(true);
}

void USAEHandComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USAEHandComponent,HandState);
}

bool USAEHandComponent::IsCarringAnyQuickbarItem() const
{
	if(IsHandEmpty())
	{
		return false;
	}
	if(HandState.Source != ESAEHandItemSource::Quickbar)
	{
		return false;
	}
	if(!HandState.ItemInstanceId.IsValid())
	{
		return false;
	}
	return true;
}


bool USAEHandComponent::IsCarringCertainQuickbarItem(FGuid ItemInstanceId) const
{
	if(!IsHandEmpty())
	{
		return false;
	}
	if(HandState.Source != ESAEHandItemSource::Quickbar)
	{
		return false;
	}
	if(HandState.ItemInstanceId != ItemInstanceId)
	{
		return false;
	}
	return true;
}


//作用域：服务端
bool USAEHandComponent::TryOccupyWithQuickbarItem(FGuid ItemInstanceId,int32 QuickbarIndex)
{
	ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	if(!PlayerState
		|| !PlayerState->HasAuthority()
		|| !ItemInstanceId.IsValid()
		|| !IsHandEmpty()
		|| IsCarringCertainQuickbarItem(ItemInstanceId))
	{
		return false;
	}

	USAEQuickbarComponent* Quickbar = PlayerState->GetQuickbarComponent();
	if(!Quickbar || !Quickbar->HasFreeSlot() || !Quickbar->LockSelectionAddUnequip())
	{
		return false;
	}

	//如果当前手持快捷栏道具，道具仍然留在快捷栏，只是取消表现
	HandState = FSAEHandState();
	HandState.Source = ESAEHandItemSource::Quickbar;
	HandState.ItemDefinitionId = 0;
	HandState.ItemInstanceId = ItemInstanceId;
	HandState.QuickbarIndex = QuickbarIndex;

	RefreshGameplayTags();
	NotifyPresentationChanged();
	PlayerState->ForceNetUpdate();
	return true;
}

//作用域：服务端
bool USAEHandComponent::TryOccupyWithStolenEgg(int32 ItemDefinitionId)
{
	ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	if(!PlayerState
		|| !PlayerState->HasAuthority()
		|| IsCarringStolenEgg()
		|| ItemDefinitionId <= INDEX_NONE)
	{
		return false;
	}

	USAEQuickbarComponent* Quickbar = PlayerState->GetQuickbarComponent();
	if(!Quickbar || !Quickbar->HasFreeSlot() || !Quickbar->LockSelectionAddUnequip())
	{
		return false;
	}

	//如果当前手持快捷栏道具，道具仍然留在快捷栏，只是取消表现
	HandState = FSAEHandState();
	HandState.Source = ESAEHandItemSource::StolenEgg;
	HandState.ItemDefinitionId = ItemDefinitionId;
	HandState.QuickbarIndex = INDEX_NONE;

	RefreshGameplayTags();
	NotifyPresentationChanged();
	PlayerState->ForceNetUpdate();
	return true;
}

//作用域：服务端
bool USAEHandComponent::ReleaseStolenEgg()
{
	ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	if(!PlayerState || !PlayerState->HasAuthority() || !IsCarringStolenEgg())
	{
		return false;
	}
	HandState = FSAEHandState();
	if(USAEQuickbarComponent* Quickbar = PlayerState->GetQuickbarComponent())
	{
		Quickbar->UnlockSelection();
	}
	RefreshGameplayTags();
	NotifyPresentationChanged();
	PlayerState->ForceNetUpdate();
	return true;
}

//作用域：服务端
void USAEHandComponent::RefreshGameplayTags()
{
	ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	UAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetAbilitySystemComponent() : nullptr;
	if(!PlayerState || !PlayerState->HasAuthority() || !ASC)
	{
		return;
	}
	const int32 HasItemCount = IsCarringAnyQuickbarItem() ? 1 : 0;
	const int32 HasStolenEggCount = IsCarringStolenEgg() ? 1 : 0;

	ASC->SetLooseGameplayTagCount(SAEGameplayTags::State_Carrying_Item,HasItemCount,EGameplayTagReplicationState::TagOnly);
	ASC->SetLooseGameplayTagCount(SAEGameplayTags::State_Carrying_StolenEgg,HasStolenEggCount,EGameplayTagReplicationState::TagOnly);
	ASC->SetLooseGameplayTagCount(SAEGameplayTags::Pose_Carry_TwoHanded,HasStolenEggCount,EGameplayTagReplicationState::TagOnly);
}

//作用域：客户端
void USAEHandComponent::OnRep_HandeState()
{
	NotifyPresentationChanged();
}

//作用域：客户端&服务端
void USAEHandComponent::NotifyPresentationChanged()
{
	const ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	ASAEPlayerCharacter* Character = PlayerState ? Cast<ASAEPlayerCharacter>(PlayerState->GetPawn()) : nullptr;
	if(!Character)
	{
		return;
	}
	//通知表现层
	if(USAEHeldItemPresentationComponent* Presentation = Character->GetHeldItemPresentationComponent())
	{
		Presentation->RefreshPresentation();
	}
}

bool USAEHandComponent::ClearHand()
{
	ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	if(!PlayerState || !PlayerState->HasAuthority())
	{
		return false;
	}
	HandState = FSAEHandState();
	if(USAEQuickbarComponent* Quickbar = PlayerState->GetQuickbarComponent())
	{
		Quickbar->UnlockSelection();
	}
	RefreshGameplayTags();
	NotifyPresentationChanged();
	PlayerState->ForceNetUpdate();
	return true;
}
