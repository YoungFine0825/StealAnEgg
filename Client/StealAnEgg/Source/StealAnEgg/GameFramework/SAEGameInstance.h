#pragma once
/*
 * 程序级生命周期入口
 * 全局错误处理
 * 保存跨地图临时流程状态
 * 提供全局系统(GameInstanceSubsystem)访问入口
 * 
 */
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SAEGameInstance.generated.h"

UCLASS()
class STEALANEGG_API USAEGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	
	virtual void Init() override;
	virtual void Shutdown() override;

private:
	bool bInitialized = false;
};
