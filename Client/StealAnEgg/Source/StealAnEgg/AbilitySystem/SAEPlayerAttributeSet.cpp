#include "SAEPlayerAttributeSet.h"

#include "Net/UnrealNetwork.h"

USAEPlayerAttributeSet::USAEPlayerAttributeSet()
{
	InitBaseMoveSpeed(600.0f);
	InitMoveSpeedBonusRate(0.0f);
	InitTrainingBonusRate(0.0f);
}

void USAEPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(//指定复制规则和何时调用属性所属对象的 OnRep
		USAEPlayerAttributeSet,
		BaseMoveSpeed,
		COND_None,
		REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(
		USAEPlayerAttributeSet,
		MoveSpeedBonusRate,
		COND_None,
		REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(
	USAEPlayerAttributeSet,
		TrainingBonusRate,
		COND_None,
		REPNOTIFY_Always);
}

void USAEPlayerAttributeSet::OnRep_BaseMoveSpeed(const FGameplayAttributeData& OldValue)
{
	//客户端收到属性复制后，通知 GAS 更新该属性的内部状态
	GAMEPLAYATTRIBUTE_REPNOTIFY(USAEPlayerAttributeSet,BaseMoveSpeed,OldValue);
}

void USAEPlayerAttributeSet::OnRep_MoveSpeedBonusRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USAEPlayerAttributeSet,MoveSpeedBonusRate,OldValue);
}

void USAEPlayerAttributeSet::OnRep_TrainingBonusRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USAEPlayerAttributeSet,TrainingBonusRate,OldValue);
}


