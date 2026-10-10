#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SAEHUD.generated.h"

class UUserWidget;

UCLASS()
class STEALANEGGUI_API ASAEHUD : public AHUD
{
	GENERATED_BODY()
public:

protected:

	virtual void BeginPlay() override;

	virtual  void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly,Category="UI")
	TSubclassOf<UUserWidget> PlayerHUDWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> PlayerHUDWidget;
	
};
