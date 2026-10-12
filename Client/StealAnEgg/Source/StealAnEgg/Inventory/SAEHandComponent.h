#pragma once

/*
 * 表示玩家当前唯一的手持物品(道具实例，非具体的Actor)，同时服务于快捷栏道具和未结算蛋
 * 
 */

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/SAEItemTypes.h"
#include "SAEHandComponent.generated.h"


UCLASS()
class STEALANEGG_API USAEHandComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USAEHandComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	const FSAEHandState GetHandState() const
	{
		return HandState;
	}
	
	const FSAEHandState& GetHandStateRef() const
	{
		return HandState;
	}

	UFUNCTION(BlueprintPure)
	bool IsHandEmpty() const
	{
		return HandState.IsEmpty();
	}

	UFUNCTION(BlueprintCallable)
	bool IsCarringAnyQuickbarItem() const;

	UFUNCTION(BlueprintCallable)
	bool IsCarringCertainQuickbarItem(FGuid ItemInstanceId) const;

	UFUNCTION(BlueprintCallable)
	bool TryOccupyWithQuickbarItem(FGuid ItemInstanceId,int32 QuickbarIndex);

	UFUNCTION(BlueprintCallable)
	bool IsCarringStolenEgg() const
	{
		return HandState.Source == ESAEHandItemSource::StolenEgg;
	}

	UFUNCTION(BlueprintCallable)
	bool TryOccupyWithStolenEgg(int32 ItemDefinitionId);

	UFUNCTION(BlueprintCallable)
	bool ReleaseStolenEgg();

	UFUNCTION(BlueprintCallable)
	bool ClearHand();
	
private:

	UFUNCTION()
	void OnRep_HandeState();

	void RefreshGameplayTags();
	void NotifyPresentationChanged();

	UPROPERTY(ReplicatedUsing=OnRep_HandeState)
	FSAEHandState HandState;

	
};
