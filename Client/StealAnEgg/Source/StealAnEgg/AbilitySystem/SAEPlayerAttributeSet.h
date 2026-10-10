#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SAEPlayerAttributeSet.generated.h"


#define SAE_ATTRIBUTE_ACCESSORS(ClassName,PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/* 宏会生成:
	GetBaseMoveSpeedAttribute() // 属性标识，用于监听和修改
	GetBaseMoveSpeed()          // 当前数值
	SetBaseMoveSpeed(...)
	InitBaseMoveSpeed(...)
 */

UCLASS()
class STEALANEGG_API USAEPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	USAEPlayerAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_BaseMoveSpeed,Category="Attributes")
	FGameplayAttributeData BaseMoveSpeed;

	SAE_ATTRIBUTE_ACCESSORS(USAEPlayerAttributeSet, BaseMoveSpeed)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MoveSpeedBonusRate,Category="Attributes")
	FGameplayAttributeData MoveSpeedBonusRate;

	SAE_ATTRIBUTE_ACCESSORS(USAEPlayerAttributeSet, MoveSpeedBonusRate)
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_TrainingBonusRate,Category="Attributes")
	FGameplayAttributeData TrainingBonusRate;

	SAE_ATTRIBUTE_ACCESSORS(USAEPlayerAttributeSet, TrainingBonusRate)
	
protected:

	UFUNCTION()
	void OnRep_BaseMoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeedBonusRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_TrainingBonusRate(const FGameplayAttributeData& OldValue);
	
private:
	

};
