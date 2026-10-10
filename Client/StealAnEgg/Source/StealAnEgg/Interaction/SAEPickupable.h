#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Interface.h"
#include "SAEPickupable.generated.h"

class AActor;
class ASAEPlayerCharacter;

/*
 * FSAEPickupable描述拾取信息
 */
USTRUCT(BlueprintType)
struct STEALANEGG_API FSAEPickupInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemDefinitionId = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer PickupTags;
	
	bool IsValid() const
	{
		return ItemDefinitionId > INDEX_NONE && Quantity > 0;		
	}
};

UINTERFACE(BlueprintType,meta=(CannotImplementInterfaceInBlueprint))
class STEALANEGG_API USAEPickupable : public UInterface
{
	GENERATED_BODY()
};

/*
 * 可拾取对象抽象接口
 * 接口禁止由纯蓝图实现，目的是保证服务器提交路径存在可靠的C++实现。
 */
class ISAEPickupable
{
	GENERATED_BODY()

public:


	//只读查询，不改变世界或库存状态
	virtual FSAEPickupInfo GetPickupInfo() const = 0;

	//客户端预检查和服务器最终检查都会调用
	virtual bool CanBePickedUpBy(const ASAEPlayerCharacter* Character,FText& OutFailureReason) const = 0;

	//只能由服务器调用；成功后由目标改变状态或销毁自己
	virtual bool CommitPickup(ASAEPlayerCharacter* Character, FText& OutFailureReason) = 0;
};


UCLASS()
class STEALANEGG_API USAEPickupableStatic : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static TScriptInterface<ISAEPickupable> GetFirstPickupableFromActor(AActor* Actor);
};