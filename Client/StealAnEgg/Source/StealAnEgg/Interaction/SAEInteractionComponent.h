#pragma once

/*
 * 输入事件桥接
 *
 * 
 * 从玩家视角寻找可交互对象，为提示UI和Ability提供候选Actor
 * 交互开始后锁定本次目标，旋转镜头不会切换目标
 */

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAEInteractionComponent.generated.h"

UCLASS(ClassGroup=(Interaction),meta=(BlueprintSpawnableComponent))
class STEALANEGG_API USAEInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USAEInteractionComponent();

	UFUNCTION(BlueprintCallable)
	AActor* FindBestInteractable() const;

private:

	UPROPERTY(EditDefaultsOnly)
	float InteractionRange = 250.0f;

	UPROPERTY(EditDefaultsOnly)
	float TraceRadius = 20.0f;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
};
