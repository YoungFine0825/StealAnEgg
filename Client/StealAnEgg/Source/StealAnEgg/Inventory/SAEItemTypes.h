#pragma once

/*
 * 定义能够跨网络复制、写入存档且不依赖世界Actor生命周期的物品实例。
 * 快捷栏中保存的将是FSAEItemInstance而不是Actor。
 * 
 */
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SAEItemTypes.generated.h"

//道具类型
UENUM(BlueprintType)
enum class ESAEItemType : uint8
{
	None,
	Egg,
	Pet,
	Weapon,
	Trap
};

//玩家手持物品来源
UENUM(BlueprintType)
enum class ESAEHandItemSource : uint8
{
	None,
	Quickbar,//快捷栏道具
	StolenEgg//偷的蛋
};

//玩家手持道具方式
UENUM()
enum class ESAEHeldPresentationMode : uint8
{
	None,
	FrontCarry,//双手持身体正前方
	AttachmentSlot,//附加到指定骨骼槽位
};

USTRUCT(BlueprintType)
struct STEALANEGG_API FSAEItemInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGuid InstanceId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 DefinitionId = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESAEItemType ItemType = ESAEItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Quantity = 0;

	bool IsValid() const
	{
		return InstanceId.IsValid() && DefinitionId != INDEX_NONE && ItemType != ESAEItemType::None;
	}

	static FSAEItemInstance MakeEgg(int32 Id,int32 InQuality)
	{
		FSAEItemInstance R;
		R.InstanceId = FGuid::NewGuid();
		R.DefinitionId = Id;
		R.ItemType = ESAEItemType::Egg;
		R.Quantity = InQuality;
		return R;
	}
};


USTRUCT(BlueprintType)
struct STEALANEGG_API FSAEQuickbarSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bOccupied = false;

	UPROPERTY(BlueprintReadOnly)
	FSAEItemInstance Item;
};

//玩家手持方式配置
USTRUCT(BlueprintType)
struct STEALANEGG_API FSAEHeldPresentationConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESAEHeldPresentationMode Mode = ESAEHeldPresentationMode::AttachmentSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttachmentSlotTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform RelativeTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> HeldActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag HeldPoseTag;
};

//玩家手持物状态
USTRUCT(BlueprintType)
struct STEALANEGG_API FSAEHandState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ESAEHandItemSource Source = ESAEHandItemSource::None;

	//当前玩家手持的道具的配置表ID
	UPROPERTY(BlueprintReadOnly)
	int32 ItemDefinitionId = 0;

	//当前玩家手持道具的实例ID
	//如果当前玩家当手持偷来的蛋，该字段没有值，因为蛋还未入库。
	UPROPERTY(BlueprintReadOnly)
	FGuid ItemInstanceId;

	UPROPERTY(BlueprintReadOnly)
	int32 QuickbarIndex = INDEX_NONE;

	bool IsEmpty() const
	{
		return Source == ESAEHandItemSource::None;
	}
};
