#include "SAEPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/SAEPlayerCharacter.h"
#include "AbilitySystem/SAEAbilitySystemComponent.h"
#include "SAEPlayerState.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Inventory/SAEStolenEggComponent.h"
#include "Inventory/SAEQuickbarComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogASAEPlayerController,Log,All);

ASAEPlayerController::ASAEPlayerController()
{
	
}

void ASAEPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocalController())
	{
		ExitMouseLookMode();

		NotifyLocalPlayerStateReady();
	}
}

void ASAEPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if(IsLocalController())//只为本地玩家初始化输入系统
	{
		if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(MouseLookHoldAction,ETriggerEvent::Started,this,&ThisClass::HandleMouseLookStarted);
			EnhancedInputComponent->BindAction(MouseLookHoldAction,ETriggerEvent::Completed,this,&ThisClass::HandleMouseLookReleased);
			EnhancedInputComponent->BindAction(MouseLookHoldAction,ETriggerEvent::Canceled,this,&ThisClass::HandleMouseLookReleased);
		}
		ULocalPlayer* LocalPlayer = GetLocalPlayer();
		if(LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if(InputSubsystem)
			{
				InputSubsystem->AddMappingContext(InputMappingContext,0);
				
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					InputSubsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void ASAEPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ASAEPlayerController::OnUnPossess()
{
	if(IsLocalController())
	{
		ExitMouseLookMode();
	}
	Super::OnUnPossess();
}

void ASAEPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
}

void ASAEPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	//Standalone、Listen Server初始化调用
	NotifyLocalPlayerStateReady();
}


void ASAEPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//客户端等到PlayerState同步完毕后，调用
	NotifyLocalPlayerStateReady();
}


ASAEPlayerCharacter* ASAEPlayerController::GetPlayerCharacter() const
{
	return Cast<ASAEPlayerCharacter>(GetCharacter());
}

ASAEPlayerState* ASAEPlayerController::GetPlayerState() const
{
	return Super::GetPlayerState<ASAEPlayerState>();
}

void ASAEPlayerController::ServerRequestInteract_Implementation(AActor* Target)
{
	
}

void ASAEPlayerController::ServerRequestSelectQuickbarSlot_Implementation(int32 slotIndex)
{
	
}

//作用域：客户端
void ASAEPlayerController::RequestDropStolenEgg()
{
	if(!IsLocalController())
	{
		return;
	}
	ServerRequestDropStolenEgg();
}


void ASAEPlayerController::ServerRequestDropStolenEgg_Implementation()
{
	ASAEPlayerState* SAEPlayerState = GetPlayerState();
	if(!IsValid(SAEPlayerState))
	{
		return;
	}
	USAEStolenEggComponent* StolenEggComponent = SAEPlayerState->GetStolenEggComponent();
	if(!IsValid(StolenEggComponent))
	{
		return;
	}
	if(!StolenEggComponent->DropStolenEgg())
	{
		ClientNotifyActionFailed(ESAEActionFailureReason::NotReady);
	}
}

void ASAEPlayerController::ServerRequestHatchEgg_Implementation(const FGuid& EggInstanceId)
{
	
}

void ASAEPlayerController::ServerRequestUnloadPet_Implementation(const FGuid& PetInstanceId)
{
	
}

void ASAEPlayerController::ServerRequestSellItem_Implementation(const FGuid& ItemInstanceId)
{
	
}

void ASAEPlayerController::ClientNotifyActionFailed_Implementation(ESAEActionFailureReason FailureReason)
{
	
}

void ASAEPlayerController::ClientNotifyPlayerReady_Implementation()
{
	
}

void ASAEPlayerController::HandleInteractInput()
{
	
}

void ASAEPlayerController::HandleDropInput()
{
	
}

void ASAEPlayerController::HandleQuickbarSlot()
{
	
}

void ASAEPlayerController::ApplyGameplayInputContext()
{
	
}

void ASAEPlayerController::RemoveGameplayInputContext()
{
	
}

void ASAEPlayerController::HandleMouseLookStarted()
{
	EnterMouseLookMode();
}

void ASAEPlayerController::HandleMouseLookReleased()
{
	ExitMouseLookMode();
}

void ASAEPlayerController::EnterMouseLookMode()
{
	if(!IsLocalController() || bMouseLookActive)
	{
		return;
	}
	bMouseLookActive = true;
	if(ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if(UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if(MoustLookMappingContext)
			{
				InputSubsystem->AddMappingContext(MoustLookMappingContext,MouseLookMappingPriority);
			}
		}
	}

	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	SetInputMode(InputMode);

	if(UGameViewportClient* ViewportClient = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
	{
		ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
		ViewportClient->SetMouseLockMode(EMouseLockMode::LockOnCapture);
	}

	UE_LOG(LogASAEPlayerController,Log,TEXT("进入鼠标观察模式"));
}

void ASAEPlayerController::ExitMouseLookMode()
{
	if(!IsLocalController())
	{
		return;
	}
	bMouseLookActive = false;
	if(ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if(UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if(MoustLookMappingContext)
			{
				InputSubsystem->RemoveMappingContext(MoustLookMappingContext);
			}
		}
	}
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	if(UGameViewportClient* ViewportClient = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
	{
		ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CaptureDuringMouseDown);
		ViewportClient->SetMouseLockMode(EMouseLockMode::DoNotLock);
	}
	
	UE_LOG(LogASAEPlayerController,Log,TEXT("退出鼠标观察模式"));
}

//通知本地客户端PlayerState就绪
void ASAEPlayerController::NotifyLocalPlayerStateReady()
{
	if(!IsLocalController())
	{
		return;
	}
	ASAEPlayerState* SAEPlayerState = GetPlayerState();
	if(!IsValid(SAEPlayerState))
	{
		return;
	}
	if(!IsValid(SAEPlayerState->GetAbilitySystemComponent()))
	{
		return;
	}
	OnLocalPlayerStateAvailable.Broadcast(SAEPlayerState);
}


void ASAEPlayerController::SAEDebugQuickbar()
{
	ASAEPlayerState* SAEPlayerState = GetPlayerState();

	USAEQuickbarComponent* Quickbar =
		IsValid(SAEPlayerState)
		? SAEPlayerState->GetQuickbarComponent()
		: nullptr;

	if(!IsValid(Quickbar))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Quickbar] PlayerState or Quickbar is not ready."));
		return;
	}

	Quickbar->DebugPrintSlots();
}
