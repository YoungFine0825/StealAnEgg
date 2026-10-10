#pragma once

/*
 * 保存当前游戏世界中玩家的权威网络数据对象
 *
 * 玩家身份摘要
 * 玩家长期属性镜像
 * 快捷栏。因为玩家因掉线、重生能原因导致Character被销毁后，快捷栏不应该被销毁。
 * 金币与成长属性
 * 玩家初始化状态
 * 宠物围栏归属信息
 * 为UI提供复制数据
 * 宠物围栏内容摘要
 * 
 */

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerState.h"
#include "GameplayAbilities/Public/AbilitySystemInterface.h"
#include "SAEPlayerState.generated.h"

class USAEQuickbarComponent;
class USAEAbilitySystemComponent;
class USAEPlayerAttributeSet;
class USAEHandComponent;
class USAEStolenEggComponent;

UENUM(BlueprintType)
enum class ESAEPlayerJoinState : uint8
{
	Connecting,
	LoadingProfile,
	AssigningPlot,
	Ready,
	Failed
};

DECLARE_MULTICAST_DELEGATE_OneParam(
	FSAEJoinStateChangedDelegate,
	ESAEPlayerJoinState);

DECLARE_MULTICAST_DELEGATE_OneParam(
	FSAEPlotChangedDelegate,
	int32);

UCLASS()
class STEALANEGG_API ASAEPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ASAEPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual  void PostInitializeComponents() override;
	
	void InitAbilitySystemAvatar(AActor* AbilitySystemAvatar);
	
	USAEQuickbarComponent* GetQuickbarComponent() const;

	USAEHandComponent* GetHandComponent() const;

	USAEStolenEggComponent* GetStolenEggComponent() const;
	
	UFUNCTION(BlueprintPure)
	ESAEPlayerJoinState GetJoinState() const
	{
		return JoinState;
	}

	UFUNCTION(BlueprintPure)
	bool IsGameplayReady() const
	{
		return JoinState == ESAEPlayerJoinState::Ready;
	}

	void SetJoinState(ESAEPlayerJoinState joinState);

	void SetAssignedPlotINdex(int32 NewPlotIndex);

	FSAEJoinStateChangedDelegate& OnJoinStateChanged()
	{
		return JoinStateChangedDelegate;
	}

	FSAEPlotChangedDelegate& OnPlotChanged()
	{
		return PlotChangedDelegate;
	}

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	
	//服务端授予默认Ability
	void GrantDefaultAbilities();
	
	UFUNCTION()
	void OnRep_JoinState();

	UFUNCTION()
	void OnRep_AssignedPlotIndex();

	USAEAbilitySystemComponent* AbilitySystemComponent;

	USAEPlayerAttributeSet* PlayerAttributeSet;

	//默认交互Ability
	UPROPERTY(EditDefaultsOnly,Category="Ability System|Startup")
	TSubclassOf<UGameplayAbility> InteractAbilityClass;

	//默认拾取Ability
	UPROPERTY(EditDefaultsOnly,Category="Ability System|Startup")
	TSubclassOf<UGameplayAbility> PickupAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player|Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="Player", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USAEQuickbarComponent> QuickbarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USAEHandComponent> HandComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USAEStolenEggComponent> StolenEggComponent;

	UPROPERTY(ReplicatedUsing = OnRep_JoinState)
	ESAEPlayerJoinState JoinState = ESAEPlayerJoinState::Connecting;

	UPROPERTY(ReplicatedUsing = OnRep_AssignedPlotIndex)
	int32 AssignedPlotIndex = INDEX_NONE;

	FString BackendAccountId;

	FSAEJoinStateChangedDelegate JoinStateChangedDelegate;
	
	FSAEPlotChangedDelegate PlotChangedDelegate;

	FGameplayAbilitySpecHandle InteractAbilityHandle;

	FGameplayAbilitySpecHandle PickupAbilityHandle;
};
