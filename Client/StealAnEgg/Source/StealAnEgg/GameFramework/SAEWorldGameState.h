#pragma once

/*
 * 当前游戏世界所有玩家都需要知道的公共状态面板
 *
 * 保存并复制当前世界阶段
 * 提供刷新周期结束时间
 * 提供世界公开信息
 * 提供PlayerArray
 * 提供稀有领地摘要信息
 */

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SAEWorldGameState.generated.h"

UENUM(BlueprintType)
enum class ESAEWorldPhase : uint8
{
	Open,
	Refreshing,
};


USTRUCT(BlueprintType)
struct FSAEWorldCycleState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ESAEWorldPhase Phase = ESAEWorldPhase::Open;

	UPROPERTY(BlueprintReadOnly)
	int32 CycleId = 0;

	UPROPERTY(BlueprintReadOnly)
	double PhaseStartServerTime = 0.0;

	UPROPERTY(BlueprintReadOnly)
	double PhaseEndServerTime = 0.0;
};

USTRUCT(BlueprintType)
struct FSAEActiveRareTerritoryInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName TerritoryName;

	UPROPERTY(BlueprintReadOnly)
	FName TarritoryDefinitionId;

	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;
};

DECLARE_MULTICAST_DELEGATE_OneParam(
		FSAEWorldCycleStateChangedDelegate,
		const FSAEWorldCycleState&);

DECLARE_MULTICAST_DELEGATE(
	FSAERareTerritoryStateChangedDelegate);

UCLASS()
class STEALANEGG_API ASAEWorldGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	const FSAEWorldCycleState& GetWorldCycleState() const
	{
		return WorldCycleState;
	}

	ESAEWorldPhase GetWorldPhase() const
	{
		return WorldCycleState.Phase;
	}

	int32 GetWorldCycleId() const
	{
		return WorldCycleState.CycleId;
	}

	bool IsTerritoryOpen() const
	{
		return WorldCycleState.Phase == ESAEWorldPhase::Open;
	}

	float GetRemainingPhaseTime() const;

	const TArray<FSAEActiveRareTerritoryInfo>& GetActiveRareTerritories() const
	{
		return ActiveRareTerritories;
	}

	void SetWorldCycleState(const FSAEWorldCycleState& InNewState);

	void SetActiveRareTerritories(const TArray<FSAEActiveRareTerritoryInfo>& NewRareTerritories);

	FSAEWorldCycleStateChangedDelegate& OnWorldCycleStateChanged()
	{
		return WorldCycleStateChangedDelegate;
	}

	FSAERareTerritoryStateChangedDelegate& OnRestoreStateChanged()
	{
		return RareTerritoryStateChangedDelegate;
	}

protected:

	UFUNCTION()
	void OnRep_WorldCycleState();

	UFUNCTION()
	void OnRep_ActiveRareTerritories();
private:

	UPROPERTY(ReplicatedUsing = OnRep_WorldCycleState)
	FSAEWorldCycleState WorldCycleState;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveRareTerritories)
	TArray<FSAEActiveRareTerritoryInfo> ActiveRareTerritories;

	FSAEWorldCycleStateChangedDelegate WorldCycleStateChangedDelegate;
	
	FSAERareTerritoryStateChangedDelegate RareTerritoryStateChangedDelegate;
};
