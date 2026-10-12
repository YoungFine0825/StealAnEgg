#include "SAEQuickbarComponent.h"

#include "Net/UnrealNetwork.h"

USAEQuickbarComponent::USAEQuickbarComponent()
{
	SetIsReplicatedByDefault(true);
}

void USAEQuickbarComponent::BeginPlay()
{
	Super::BeginPlay();
	if(GetOwner()->HasAuthority() && Slots.IsEmpty())
	{
		Slots.SetNum(Capacity);
	}
}

void USAEQuickbarComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USAEQuickbarComponent, Slots);
	DOREPLIFETIME(USAEQuickbarComponent, ActiveSlotIndex);
	DOREPLIFETIME(USAEQuickbarComponent, bSelectionLocked);
}

bool USAEQuickbarComponent::HasFreeSlot() const
{
	return Slots.ContainsByPredicate([] (const FSAEQuickbarSlot& S){return !S.bOccupied;});
}

//只允许服务器添加道具
bool USAEQuickbarComponent::TryAddItem(const FSAEItemInstance& Item)
{
	if(!GetOwner()->HasAuthority() || !Item.IsValid())
	{
		return false;
	}
	FSAEQuickbarSlot* S = Slots.FindByPredicate( [] (const FSAEQuickbarSlot& S){return !S.bOccupied;});
	if(!S)
	{
		return false;
	}
	S->bOccupied = true;
	S->Item = Item;
	OnChanged.Broadcast();
	GetOwner()->ForceNetUpdate();
	return true;
}

bool USAEQuickbarComponent::TryAddItemByDefinition(int32 ItemDefinitionId, int32 Quantity, FSAEItemInstance* OutCreatedItemInstance)
{
	AActor* Owner = GetOwner();
	if(!Owner || !Owner->HasAuthority())
	{
		return false;
	}
	if(ItemDefinitionId <= INDEX_NONE || Quantity <= 0 || !HasFreeSlot())
	{
		return false;
	}
	
	FSAEItemInstance NewInstance;
	NewInstance.DefinitionId = ItemDefinitionId;
	NewInstance.InstanceId = FGuid::NewGuid();
	NewInstance.Quantity = Quantity;
	NewInstance.ItemType = ESAEItemType::Egg;
	
	if(!TryAddItem(NewInstance))
	{
		return false;
	}
	if(OutCreatedItemInstance)
	{
		*OutCreatedItemInstance = NewInstance;
	}
	return true;
}


//只允许服务端调用
bool USAEQuickbarComponent::TrySelectSlot(int32 SlotIndex)
{
	if(!GetOwner()->HasAuthority() || bSelectionLocked || !Slots.IsValidIndex(SlotIndex))
	{
		return false;
	}
	ActiveSlotIndex = SlotIndex;
	OnChanged.Broadcast();
	return true;
}

//只允许服务端调用
bool USAEQuickbarComponent::LockSelectionAddUnequip()
{
	if(!GetOwner()->HasAuthority() || bSelectionLocked)
	{
		return false;
	}
	bSelectionLocked = true;
	ActiveSlotIndex = INDEX_NONE;
	OnChanged.Broadcast();
	return true;
}

void USAEQuickbarComponent::UnlockSelection()
{
	if(GetOwner()->HasAuthority())
	{
		bSelectionLocked = false;
		OnChanged.Broadcast();
	}
}

void USAEQuickbarComponent::DebugPrintSlots() const
{
	const AActor* OwnerActor = GetOwner();

	UE_LOG(LogTemp, Display,
		TEXT("[Quickbar] Owner=%s Authority=%d Slots=%d ActiveSlot=%d Locked=%d"),
		*GetNameSafe(OwnerActor),
		OwnerActor && OwnerActor->HasAuthority(),
		Slots.Num(),
		ActiveSlotIndex,
		bSelectionLocked);

	for(int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		const FSAEQuickbarSlot& Slot = Slots[Index];

		if(!Slot.bOccupied)
		{
			UE_LOG(LogTemp, Display,
				TEXT("[Quickbar] Slot[%d]: Empty"), Index);
			continue;
		}

		const FSAEItemInstance& Item = Slot.Item;

		UE_LOG(LogTemp, Display,
			TEXT("[Quickbar] Slot[%d]: DefinitionId=%d Quantity=%d InstanceId=%s"),
			Index,
			Item.DefinitionId,
			Item.Quantity,
			*Item.InstanceId.ToString());
	}
}

TArray<FSAEQuickbarSlot> USAEQuickbarComponent::GetOcuppiedSlots() const
{
	TArray<FSAEQuickbarSlot> Result;
	for(const FSAEQuickbarSlot& Slot : Slots)
	{
		if(Slot.bOccupied)
		{
			Result.Add(Slot);
		}
	}
	return Result;
}




