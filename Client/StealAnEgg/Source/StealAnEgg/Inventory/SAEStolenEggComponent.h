#pragma once

/*
 * 处理玩家偷蛋数据逻辑，保存玩家当前正在偷的蛋。挂在PlayerState上。
 * 
 */
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAEStolenEggComponent.generated.h"

class ASAEWorldEgg;

UCLASS()
class STEALANEGG_API  USAEStolenEggComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USAEStolenEggComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	ASAEWorldEgg* GetCarriedEgg() const
	{
		return CarriedEgg.Get();
	}

	UFUNCTION(BlueprintPure)
	bool IsCarriedEgg() const
	{
		return GetCarriedEgg() != nullptr;
	}

	UFUNCTION(BlueprintCallable)
	bool TryCarryEgg(ASAEWorldEgg* Egg);

	UFUNCTION(BlueprintCallable)
	bool DropStolenEgg();

	UFUNCTION(BlueprintCallable)
	bool CommitStolenEgg();

private:

	UFUNCTION()
	void OnRep_CarriedEgg();

	FVector CalcuteDropLocation() const;

	void NotifyPresentationChanged();

	//当前玩家手持（偷）的蛋
	UPROPERTY(ReplicatedUsing = ONRep_CarriedEgg)
	TObjectPtr<ASAEWorldEgg> CarriedEgg;

	FVector LastValidLocation = FVector::ZeroVector;
};
