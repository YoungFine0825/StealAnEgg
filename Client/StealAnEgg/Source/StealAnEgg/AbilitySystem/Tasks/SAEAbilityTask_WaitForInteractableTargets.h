#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Interaction/SAEInteractionOption.h"
#include "SAEAbilityTask_WaitForInteractableTargets.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSAEInteractionOptionsChanged,
	const TArray<FSAEInteractionOption>&, Options);

UCLASS()
class STEALANEGG_API USAEAbilityTask_WaitForInteractableTargets : public UAbilityTask
{
	GENERATED_BODY()

public:
	
	static USAEAbilityTask_WaitForInteractableTargets* WaitForTargets(
		UGameplayAbility* OwningAbility,
		float ScanRange,
		float ScanRate,
		float TraceRadius,
		ECollisionChannel TraceChannel);

	UPROPERTY(BlueprintAssignable)
	FSAEInteractionOptionsChanged OnOptionsChanged;

	virtual void Activate() override;

protected:
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:

	void PerformScan();

	void AppendTarget(UObject* Object,TArray<TScriptInterface<ISAEInteractableTarget>>& OutTargets,TSet<UObject*>& SeenObjects) const;

	static bool AreOptionsEqual(
		const TArray<FSAEInteractionOption>& Left,
		const TArray<FSAEInteractionOption>& Right);

	float ScanRange = 500.0f;

	float ScanRate = 0.1f;

	float TraceRadius = 20.0f;

	ECollisionChannel TraceChannel = ECC_Visibility;

	FTimerHandle ScanTimerHandle;

	UPROPERTY()
	TArray<FSAEInteractionOption> CachedOptions;
};
