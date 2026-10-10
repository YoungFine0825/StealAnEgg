#include "SAEWorldGameMode.h"

#include "SAEPlayerController.h"
#include "SAEWorldGameState.h"

ASAEWorldGameMode::ASAEWorldGameMode()
{
	GameStateClass = ASAEWorldGameState::StaticClass();
	PlayerControllerClass = ASAEPlayerController::StaticClass();
}

void ASAEWorldGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ASAEWorldGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeWorldSystems();
}

void ASAEWorldGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ASAEPlayerController* PlayerController = Cast<ASAEPlayerController>(NewPlayer);
	if(!PlayerController)
	{
		return;;
	}
	BeginPlayerDataLoad(PlayerController);
}

void ASAEWorldGameMode::Logout(AController* Exiting)
{
	if(ASAEPlayerController* PlayerController = Cast<ASAEPlayerController>(Exiting))
	{
		CleanupLeavingPlayers(PlayerController);
	}
	Super::Logout(Exiting);
}

void ASAEWorldGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// 不在 Super 调用后立刻生成角色。
	// 等玩家数据加载和区域分配完成后，
	// 再通过 CompletePlayerJoin/RestartPlayer 生成。
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

AActor* ASAEWorldGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	return Super::ChoosePlayerStart_Implementation(Player);
}

bool ASAEWorldGameMode::IsTerritoryInteractionAllowed(AActor* Player)
{
	return true;
}


void ASAEWorldGameMode::InitializeWorldSystems()
{
	
}

void ASAEWorldGameMode::BeginPlayerDataLoad(ASAEPlayerController* Player){}

void ASAEWorldGameMode::CompletePlayerJoin(ASAEPlayerController* Player){}

void ASAEWorldGameMode::HandlePlayerJoinFailed(ASAEPlayerController* Player){}

void ASAEWorldGameMode::HandleRefreshStarted()
{
	
}

void ASAEWorldGameMode::HandleRefreshCompleted(){}

void ASAEWorldGameMode::ReturnPlayersToSafeZoneForRefresh(){}

void ASAEWorldGameMode::CleanupLeavingPlayers(ASAEPlayerController* Player){}


