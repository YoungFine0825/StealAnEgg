#pragma once

/*
 * 游戏世界中，可被拾取的蛋的实体
 * 
 */

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SAEInteractableTarget.h"
#include "Interaction/SAEPickupable.h"
#include "SAEWorldEgg.generated.h"

class ASAEPlayerCharacter;
class UStaticMeshComponent;
class UChildActorComponent;
class USphereComponent;
class USAEGA_Pickup;

UENUM(BlueprintType)
enum class ESAEWorldEggState : uint8
{
	Available,
	Carried,
	Dropped,
	Settled
};

UCLASS()
class STEALANEGG_API ASAEWorldEgg : public AActor,public ISAEInteractableTarget,public ISAEPickupable
{
	GENERATED_BODY()
public:
	ASAEWorldEgg();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	virtual bool CanInteract(const ASAEPlayerCharacter* Character, FText& OutFailureReason) const override;

	UFUNCTION(BlueprintPure)
	virtual float GetInteractionDuration() const override
	{
		return PickDuration;
	}

	virtual  FSAEPickupInfo GetPickupInfo() const override;

	virtual bool CanBePickedUpBy(const ASAEPlayerCharacter* Character, FText& OutFailureReason) const override;

	virtual bool CommitPickup(ASAEPlayerCharacter* Character, FText& OutFailureReason) override;

	virtual void GatherInteractionOptions(const ASAEPlayerCharacter* Character, FSAEInteractionOptionBuilder& Builder) const override;
	
	//服务器拾取逻辑
	UFUNCTION(BlueprintCallable)
	bool TryPickup(ASAEPlayerCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void DropAt(const FVector& Location);

	UFUNCTION(BlueprintPure)
	bool IsAvailable() const
	{
		return EggState == ESAEWorldEggState::Available || EggState == ESAEWorldEggState::Dropped;
	}

	UFUNCTION(BlueprintPure)
	int32 GetItemDefinitionId() const
	{
		return ItemDefinitionId;
	}

	UFUNCTION(BlueprintCallable)
	bool ResetItemDefinitionId(int32 InItemDefinitionId);

	void ConsumeAfterSettlement();

	UFUNCTION(BlueprintPure)
	int32 GetEggDefinitionId() const
	{
		return CurrentEggDefinitionId;
	}

protected:

	virtual void BeginPlay() override;

	//C++侧不读取配置表，由蓝图侧读取Egg配置信息
	UFUNCTION(BlueprintImplementableEvent,Category="Egg|Config")
	bool ResolveEggConfigByBlueprint();

	UFUNCTION(BlueprintCallable,Category="Egg|Appearance")
	bool CanCreateEggAppearance() const;
	
	UFUNCTION(BlueprintNativeEvent,Category="Egg|Appearance")
	void CreateEggAppearance(int32 EggDefinitionId);

	virtual void CreateEggAppearance_Implementation(int32 EggDefinitionId);

	//道具配置表ID-> DT_InventoryConfigs
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Egg",ReplicatedUsing=OnRep_ItemDefinitionId)
	int32 ItemDefinitionId = INDEX_NONE;

	//蛋配置表ID-> DT_EggConfigs
	UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_CurrentEggDefinitionId)
	int32 CurrentEggDefinitionId = INDEX_NONE;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Egg")
	int32 CurrentEggQuality = 0;

	// 必须配置为实际授予玩家的 GA_Pickup 蓝图子类
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Egg|Interaction")
	TSubclassOf<USAEGA_Pickup> PickupAbilityClass;
	
private:

	UFUNCTION()
	void OnRep_EggState();

	UFUNCTION()
	void OnRep_ItemDefinitionId();

	UFUNCTION()
	void OnRep_CurrentEggDefinitionId();

	void RefreshPresentation();

	UPROPERTY(VisibleAnywhere,Category="Egg",meta=(AllowPrivateAccess=true))
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Appearence",meta=(AllowPrivateAccess=true))
	TObjectPtr<UChildActorComponent> AppearenceComponent;

	UPROPERTY(VisibleAnywhere,Category="Appearence",meta=(AllowPrivateAccess=true))
	TObjectPtr<USphereComponent> CollisionComponent;
	
	UPROPERTY(EditDefaultsOnly,Category="Interaction")
	float PickDuration = 2.0f;

	UPROPERTY(ReplicatedUsing=OnRep_EggState)
	ESAEWorldEggState EggState = ESAEWorldEggState::Available;

	UPROPERTY(Replicated)
	TObjectPtr<APlayerState> CarryingPlayer;
	
	float MaximumPickupDistance = 300.0f;
};
