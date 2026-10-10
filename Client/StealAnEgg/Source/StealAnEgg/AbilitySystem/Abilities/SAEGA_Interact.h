#pragma once

/*
 * 负责扫描场景内可交互物体，运行在本地
 * 
 */

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Interaction/SAEInteractionOption.h"
#include "SAEGA_Interact.generated.h"

class USAEAbilityTask_WaitForInteractableTargets;
struct FAbilityEndedData;

UCLASS(Abstract,Blueprintable)
class STEALANEGG_API USAEGA_Interact : public UGameplayAbility
{
	GENERATED_BODY()
public:
	USAEGA_Interact();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec) override;
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	virtual void InputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(BlueprintCallable)
	void SetSelectedOptionIndex(int32 NewIndex);

protected:

	UFUNCTION()
	void HandleOptionsChanged(const TArray<FSAEInteractionOption>& Options);

	void HandleAbilityEnded(const FAbilityEndedData& EndedData);

	bool TriggerInteraction();

	void ReleaseInteraction();

	//蓝图扩展点
	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractionOptionsChanged(const TArray<FSAEInteractionOption>& Options,int32 SelectedIndex);

	//蓝图扩展点
	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractionActivationFailed(const FSAEInteractionOption& Options);

	UPROPERTY(EditDefaultsOnly,Category="Interaction")
	float ScanRange = 500.0f;

	UPROPERTY(EditDefaultsOnly,Category="Interaction")
	float ScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly,Category="Interaction")
	float TraceRadius = 20.0f;

	UPROPERTY(EditDefaultsOnly,Category="Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	
	UPROPERTY(BlueprintReadOnly,Category="Interaction")
	TArray<FSAEInteractionOption> CurrentOptions;

	
	UPROPERTY(BlueprintReadOnly,Category="Interaction")
	int32 SelectedOptionIndex = INDEX_NONE;

private:

	void BroadcastInteractionOptionsChanged(const TArray<FSAEInteractionOption>& Options,int32 SelectedIndex);

	UPROPERTY()
	TObjectPtr<USAEAbilityTask_WaitForInteractableTargets> ScanTasks;
	
	FGameplayAbilitySpecHandle ActiveInteractionAbilityHandle;

	FDelegateHandle AbilityEndedDelegateHandle;
};
