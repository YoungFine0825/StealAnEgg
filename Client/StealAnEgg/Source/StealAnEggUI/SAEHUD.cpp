#include "SAEHUD.h"

#include "Blueprint/UserWidget.h"

void ASAEHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	if(!PC || !PC->IsLocalController() || !PlayerHUDWidgetClass)
	{
		return;
	}

	PlayerHUDWidget = CreateWidget<UUserWidget>(PC,PlayerHUDWidgetClass);
	if(PlayerHUDWidget)
	{
		PlayerHUDWidget->AddToPlayerScreen();
	}
}

void ASAEHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(PlayerHUDWidget)
	{
		PlayerHUDWidget->RemoveFromParent();
		PlayerHUDWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}


