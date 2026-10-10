#pragma once
/*
 * 通用拾取能力
 * 
 */
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Interaction/SAEPickupable.h"
#include "SAEGA_Pickup.generated.h"

class AActor;
class ASAEPlayerCharacter;
class UAbilityTask_WaitDelay;
class UAbilityTask_WaitInputRelease;

UCLASS(Abstract, Blueprintable)
class STEALANEGG_API USAEGA_Pickup : public UGameplayAbility
{
	GENERATED_BODY()
public:
	USAEGA_Pickup();

protected:

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	
	/*预测客户端读条完成，不代表服务器已经接受拾取*/
	UFUNCTION(BlueprintImplementableEvent,Category="SAE|Pickup")
	void OnPickupPredictedComplete(AActor* PickupActor,const FSAEPickupInfo& PickupInfo);

	/* Standalone或ListenServer主机提交成功时调用*/
	UFUNCTION(BlueprintImplementableEvent,Category="SAE|Pickup")
	void OnPickupCommitted(AActor* PickupActor,const FSAEPickupInfo& PickupInfo);

	/* 蓝图扩展点 Start*/
	UFUNCTION(BlueprintImplementableEvent,Category="SAE|Pickup")
	void OnPickupStarted(AActor* PickupActor,const FSAEPickupInfo& PickupInfo,float Duration);
	
	UFUNCTION(BlueprintImplementableEvent,Category="SAE|Pickup")
	void OnPickupSucceeded(AActor* PickupActor);
	
	UFUNCTION(BlueprintImplementableEvent,Category="SAE|Pickup")
	void OnPickupFailed(AActor* PickupActor,const FText& FailureReason);

	UFUNCTION(BlueprintImplementableEvent,Category="SAE|Pickup")
	void OnPickupCancelled(AActor* PickupActor);
	/* 蓝图扩展点 End*/
	
private:

	UFUNCTION()
	void HandleDurationFinished();

	UFUNCTION()
	void HandleInputRelease(float TimeHeld);

	bool ResolveAndValidateTarget(const FGameplayEventData* TriggerEvent,FText& OutFailureReason);

	void FailAndEnd(const FText& FailureReason);

	ASAEPlayerCharacter* GetSAECharacter() const;

	bool IsLocallyControlledAbility() const;

	bool IsAuthorityAbility() const;

	UPROPERTY()
	TObjectPtr<AActor> PickupTargetActor;

	UPROPERTY()
	TScriptInterface<ISAEPickupable> PickupTarget;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> WaitDurationTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputRelease> WaitInputReleaseTask;

	FSAEPickupInfo CachedPickupInfo;

	bool bPickupStarted = false;

	bool bDurationFinished = false;

	bool bSuppressCancelEvent = false;
};
