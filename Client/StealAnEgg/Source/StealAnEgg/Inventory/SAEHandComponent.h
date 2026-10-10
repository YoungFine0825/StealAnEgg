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

	bool IsHandEmpty() const
	{
		return HandState.IsEmpty();
	}

	bool IsCarringStolenEgg() const
	{
		return HandState.Source == ESAEHandItemSource::StolenEgg;
	}

	bool TryOccupyWithStolenEgg(int32 ItemDefinitionId);

	bool ReleaseStolenEgg();
private:

	UFUNCTION()
	void OnRep_HandeState();

	void RefreshGameplayTags();
	void NotifyPresentationChanged();

	UPROPERTY(ReplicatedUsing=OnRep_HandeState)
	FSAEHandState HandState;

	
};
