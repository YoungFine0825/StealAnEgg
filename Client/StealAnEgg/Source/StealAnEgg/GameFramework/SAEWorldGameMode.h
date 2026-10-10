#pragma once

/*
 * 当前DS游戏世界的服务器总协调者
 * DS游戏世界是一个持续运行的世界，没有传统的	WaitingToStart
 *											-> InProgress
 *											-> MatchEnded
 * 建议继承自AGameModeBase而不是AGameMode
 *
 * 初始化游戏世界
 * 控制玩家加入资格
 * 玩家数据加载完成后的进入流程
 * 分配玩家区域
 * 玩家出生和重生
 * 协调世界刷新周期
 * 处理玩家退出和掉线
 * 提供服务端规则入口
 */

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SAEWorldGameMode.generated.h"

class ASAEPlayerController;
class ASAEWorldGameState;

UCLASS()
class STEALANEGG_API ASAEWorldGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:

	ASAEWorldGameMode();
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	bool IsTerritoryInteractionAllowed(AActor* Player);

protected:
	void InitializeWorldSystems();

	void BeginPlayerDataLoad(ASAEPlayerController* Player);

	void CompletePlayerJoin(ASAEPlayerController* Player);

	void HandlePlayerJoinFailed(ASAEPlayerController* Player);

	void HandleRefreshStarted();

	void HandleRefreshCompleted();

	void ReturnPlayersToSafeZoneForRefresh();

	void CleanupLeavingPlayers(ASAEPlayerController* Player);

private:

};
