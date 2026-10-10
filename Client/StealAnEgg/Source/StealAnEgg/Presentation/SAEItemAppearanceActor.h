#pragma once

/*
 * 道具的纯表现Actor，不包含游戏逻辑。
 * 用于多个逻辑实体需要公用一套表现的情况，比如：可被偷取的蛋和放置在围栏中等待孵化的蛋，它们逻辑不一致但使用模型和材质相同。
 * 可用于蛋、武器道具、宠物
 */

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAEItemAppearanceActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class STEALANEGG_API ASAEItemAppearanceActor : public AActor
{
	GENERATED_BODY()
public:
	ASAEItemAppearanceActor();

private:

	UPROPERTY(EditAnywhere,Category="Appearance")
	TObjectPtr<USceneComponent> DefaultRoot;
	
	UPROPERTY(EditAnywhere,Category="Appearance")
	TObjectPtr<UStaticMeshComponent> StaticMesh0;	
};
