#pragma once

/*
 * 仅调用基类的AbilitySpecInputReleased()不足以唤醒UAbilityTask_WaitInputRelease。
 * 需要将输入转换为GAS通用的复制事件。
 * 
 */

#include "CoreMinimal.h"
#include "GameplayAbilities/Public/AbilitySystemComponent.h"
#include "SAEAbilitySystemComponent.generated.h"

UCLASS()
class STEALANEGG_API USAEAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;

	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
};
