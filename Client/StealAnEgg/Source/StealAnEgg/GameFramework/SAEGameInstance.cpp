#include "SAEGameInstance.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogSAEGameInstance, Log, All);

void USAEGameInstance::Init()
{
	Super::Init();
	if(bInitialized)
	{
		return;
	}
	bInitialized = true;
	UE_LOG(LogSAEGameInstance, Display, TEXT("GameInstance Initialized. DedicatedServer=%s"),IsDedicatedServerInstance() ? TEXT("true") : TEXT("false"));
}

void USAEGameInstance::Shutdown()
{
	bInitialized = false;
	Super::Shutdown();
}

