#pragma once

/*
 * 保存玩家可持久化的快捷栏内容，并控制当前选中格。
 * 不保存尚未成功偷取的临时蛋
 * 
 */
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/SAEItemTypes.h"
#include "SAEQuickbarComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSAEQuickbarChanged);

UCLASS()
class STEALANEGG_API USAEQuickbarComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USAEQuickbarComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	bool HasFreeSlot() const;

	bool TryAddItem(const FSAEItemInstance& Item);

	bool TryAddItemByDefinition(int32 ItemDefinitionId,int32 Quantity,FSAEItemInstance* OutCreatedItemInstance = nullptr);

	bool TrySelectSlot(int32 SlotIndex);

	bool LockSelectionAddUnequip();

	void UnlockSelection();

	int32 GetActiveSlotIndex() const
	{
		return ActiveSlotIndex;
	}

	void DebugPrintSlots() const;

	UPROPERTY(BlueprintAssignable)
	FSAEQuickbarChanged OnChanged;

protected:

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnRep_State()
	{
		OnChanged.Broadcast();
	}

	UPROPERTY(EditDefaultsOnly)
	int32 Capacity = 8;

	UPROPERTY(ReplicatedUsing=OnRep_State)
	TArray<FSAEQuickbarSlot> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_State)
	int32 ActiveSlotIndex = INDEX_NONE;

	UPROPERTY(ReplicatedUsing=OnRep_State)
	bool bSelectionLocked = false;
};
