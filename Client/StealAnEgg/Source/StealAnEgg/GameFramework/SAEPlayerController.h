#pragma once

/*
 * 玩家与服务器之间专属通信入口
 *
 * 接收本地玩家输入，包括不完全属于Character的输入。比如：快捷栏数字键、交互、UI输入模式切换
 * 作为Server RPC 入口
 * 验证操作上下文
 * 向所属客户端发送专属反馈
 * 管理输入模式
 * 连接后的玩家准备状态。比如：GameMode加载玩家数据期间，可以通过PlayerController禁止玩家操作、向客户端端显示加载状态
 */

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SAEPlayerController.generated.h"

class ASAEPlayerCharacter;
class ASAEPlayerState;
class UInputAction;
class UInputMappingContext;

UENUM(BlueprintType)
enum class ESAEActionFailureReason : uint8
{
	None,
	NotReady,
	InvalidTarget,
	TooFarAway,
	QuickbarFull,
	QuickbarLocked,
	AlreadyCarringEgg,
	EggNotReady,
	NotInSafeZone,
	ServerRejected
};

DECLARE_MULTICAST_DELEGATE_OneParam(
	FSAEACtionFailureDelegate,
	ESAEActionFailureReason
	);

DECLARE_MULTICAST_DELEGATE(FSAEPlayerReadyDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSAEPlayerStateAvailableSignature,
	ASAEPlayerState*, PlayerState
	);

UCLASS()
class STEALANEGG_API ASAEPlayerController : public APlayerController
{
	GENERATED_BODY()
public:

	ASAEPlayerController();
	
	virtual void BeginPlay() override;

	//本地客户端绑定玩家级输入
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	//处理玩家掉线逻辑
	virtual void PawnLeavingGame() override;

	virtual void OnRep_Pawn() override;

	virtual void InitPlayerState() override;

	virtual void OnRep_PlayerState() override;

	ASAEPlayerCharacter* GetPlayerCharacter() const;
	ASAEPlayerState* GetPlayerState() const;

	FSAEACtionFailureDelegate& OnActionFailure()
	{
		return ActionFailureDelegate;
	}

	FSAEPlayerReadyDelegate& OnPlayerReady()
	{
		return PlayerReadyDelegate;
	}

	UFUNCTION(Server, Reliable)
	void ServerRequestInteract(AActor* Target);
	
	UFUNCTION(Server, Reliable)
	void ServerRequestSelectQuickbarSlot(int32 slotIndex);

	//客户端调用此函数，请求放下蛋
	UFUNCTION(BlueprintCallable,Category="SAE|Inventory")
	void RequestDropStolenEgg();
	
	UFUNCTION(Server, Reliable)
	void ServerRequestDropStolenEgg();

	UFUNCTION(Server, Reliable)
	void ServerRequestHatchEgg(const FGuid& EggInstanceId);

	UFUNCTION(Server, Reliable)
	void ServerRequestUnloadPet(const FGuid& PetInstanceId);

	UFUNCTION(Server, Reliable)
	void ServerRequestSellItem(const FGuid& ItemInstanceId);
	
	UFUNCTION(Client, Reliable)
	void ClientNotifyActionFailed(ESAEActionFailureReason FailureReason);

	UFUNCTION(Client, Reliable)
	void ClientNotifyPlayerReady();

	UPROPERTY(BlueprintAssignable, Category="SAE|Player")
	FSAEPlayerStateAvailableSignature OnLocalPlayerStateAvailable;
	
protected:

	void HandleInteractInput();
	void HandleDropInput();

	void HandleQuickbarSlot();

	void ApplyGameplayInputContext();
	void RemoveGameplayInputContext();

	/*按住鼠标右键控制镜头相关*/
	void HandleMouseLookStarted();
	void HandleMouseLookReleased();
	void EnterMouseLookMode();
	void ExitMouseLookMode();

	void NotifyLocalPlayerStateReady();

private:

	UFUNCTION(Exec)
	void SAEDebugQuickbar();
	
private:

	FSAEACtionFailureDelegate ActionFailureDelegate;

	FSAEPlayerReadyDelegate PlayerReadyDelegate;

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	//当按住鼠标右键时，才能允许用鼠标控制镜头
	UPROPERTY(EditDefaultsOnly,Category="Input|Mouse Look")
	TObjectPtr<UInputAction> MouseLookHoldAction;

	UPROPERTY(EditDefaultsOnly,Category="Input|Mouse Look")
	TObjectPtr<UInputMappingContext> MoustLookMappingContext;

	UPROPERTY(EditDefaultsOnly,Category="Input|Mouse Look")
	int32 MouseLookMappingPriority = 10;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UInputAction> DropItemAction;

	bool bMouseLookActive;
};
