#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/SAEItemTypes.h"
#include "SAEHeldItemPresentationComponent.generated.h"

UCLASS(ClassGroup=(Presentation),meta=(BlueprintSpawnableComponent))
class STEALANEGG_API USAEHeldItemPresentationComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USAEHeldItemPresentationComponent();
	
	void RefreshPresentation();

protected:

	virtual  void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	void ShowQuickbatItem(const FSAEHandState& HandeState);

	void ClearPresentation();

	const FSAEHeldPresentationConfig* ResolvePresentationConfig(int32 DefinitionId) const;

	UPROPERTY(EditDefaultsOnly,Category="Held Item")
	TMap<int32, FSAEHeldPresentationConfig> PresentationConfigs;

	UPROPERTY(Transient)
	TObjectPtr<AActor> LocalHeldVisualActor;
};
