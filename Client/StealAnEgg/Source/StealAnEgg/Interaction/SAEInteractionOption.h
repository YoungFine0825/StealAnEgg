#pragma once

/*
 * FSAEIneractionOption描述一个交互行为，不描述物品内容。
 * 一个Actor可以同时提供“拾取”“、"检查"、”标记“等多个选项
 * 
 */

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "UObject/ScriptInterface.h"
#include "SAEInteractionOption.generated.h"

class ISAEInteractableTarget;

USTRUCT(BlueprintType)
struct STEALANEGG_API FSAEInteractionOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<ISAEInteractableTarget> Target;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag InteractionTag;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText Text;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText SubText;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Duration = 0;

	/*
	 * UI无关的显示样式标识
	 * StealAnEggUI负责将它映射到具体的USerWidget、图标、布局
	 */
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag InteractionStyleTag;

	bool operator==(const FSAEInteractionOption& Other) const
	{
		return Target.GetObject() == Other.Target.GetObject()
		&& AbilityClass == Other.AbilityClass
		&& InteractionTag == Other.InteractionTag
		&& InteractionStyleTag == Other.InteractionStyleTag
		&& Text.IdenticalTo(Other.Text)
		&& SubText.IdenticalTo(Other.SubText)
		&& FMath::IsNearlyEqual(Duration, Other.Duration);
	}

	bool operator!=(const FSAEInteractionOption& Other) const
	{
		return !(*this == Other);
	}
};
