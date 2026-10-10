#include "SAEStolenEggComponent.h"

#include "HeadMountedDisplayTypes.h"
#include "SAEHandComponent.h"
#include "SAEQuickbarComponent.h"
#include "AbilitySystem/SAEGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Player/SAEPlayerCharacter.h"
#include "GameFramework/SAEPlayerState.h"
#include "Player/SAEHeldItemPresentationComponent.h"
#include "World/Egg//SAEWorldEgg.h"
#include "Interaction/SAEInteractionMessages.h"
#include "GameFramework/GameplayMessageSubsystem.h"

USAEStolenEggComponent::USAEStolenEggComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void USAEStolenEggComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAEStolenEggComponent,CarriedEgg);
}

//作用域：服务端
//
bool USAEStolenEggComponent::TryCarryEgg(ASAEWorldEgg* Egg)
{
	ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	if(!PlayerState || !PlayerState->HasAuthority() || !Egg || CarriedEgg)
	{
		return false;
	}
	if(CarriedEgg == Egg)
	{
		return false;
	}
	USAEHandComponent* HandComp = PlayerState->GetHandComponent();
	if(!HandComp || !HandComp->TryOccupyWithStolenEgg(Egg->GetItemDefinitionId()))
	{
		return false;
	}

	const APawn* Pawn = PlayerState->GetPawn();
	LastValidLocation = Pawn ? Pawn->GetActorLocation() : Egg->GetActorLocation();
	CarriedEgg = Egg;
	NotifyPresentationChanged();
	PlayerState->ForceNetUpdate();

	return true;
}

bool USAEStolenEggComponent::DropStolenEgg()
{
	ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	if(!PlayerState || !PlayerState->HasAuthority() || !CarriedEgg)
	{
		return false;
	}
	ASAEWorldEgg* Egg = CarriedEgg;
	const FVector DropLocation = CalcuteDropLocation();
	CarriedEgg = nullptr;

	if(USAEHandComponent* Hand = PlayerState->GetHandComponent())
	{
		Hand->ReleaseStolenEgg();
	}
	Egg->DropAt(DropLocation);
	PlayerState->ForceNetUpdate();
	return true;
}

bool USAEStolenEggComponent::CommitStolenEgg()
{
	ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	if(!PlayerState || !PlayerState->HasAuthority() || !CarriedEgg)
	{
		return false;
	}
	USAEQuickbarComponent* Quickbar = PlayerState->GetQuickbarComponent();
	if(!Quickbar)
	{
		return false;
	}

	const FSAEPickupInfo PickupInfo = CarriedEgg->GetPickupInfo();
	if(!PickupInfo.IsValid() || !Quickbar->TryAddItemByDefinition(PickupInfo.ItemDefinitionId,PickupInfo.Quantity))
	{
		return false;
	}
	
	ASAEWorldEgg* Egg = CarriedEgg;
	CarriedEgg = nullptr;

	if(USAEHandComponent* Hand = PlayerState->GetHandComponent())
	{
		Hand->ReleaseStolenEgg();
	}
	Egg->ConsumeAfterSettlement();
	PlayerState->ForceNetUpdate();
	return true;
}

FVector USAEStolenEggComponent::CalcuteDropLocation() const
{
	const ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	const APawn* Pawn = PlayerState ? PlayerState->GetPawn() : nullptr;
	const FVector Origin = Pawn ? Pawn->GetActorLocation() : LastValidLocation;
	const FVector Forward = Pawn ? Pawn->GetActorForwardVector() : FVector::ForwardVector;
	const FVector TraceStart = Origin + Forward * 100.0f + FVector(0.0f, 0.0f, 80.0f);
	const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 300.0f);

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SAEDrop),false,Pawn);
	if(GetWorld()->LineTraceSingleByChannel(Hit,TraceStart,TraceEnd,ECC_Visibility,QueryParams))
	{
		return Hit.ImpactPoint + FVector(0.0f, 0.0f, 20.0f);
	}
	return Origin + Forward * 100.0f;
}

void USAEStolenEggComponent::OnRep_CarriedEgg()
{
	NotifyPresentationChanged();
}


void USAEStolenEggComponent::NotifyPresentationChanged()
{
	const ASAEPlayerState* PlayerState = Cast<ASAEPlayerState>(GetOwner());
	ASAEPlayerCharacter* Character = PlayerState ? Cast<ASAEPlayerCharacter>(PlayerState->GetPawn()) : nullptr;
	if(!Character)
	{
		return;
	}
	if(USAEHeldItemPresentationComponent* HeldItemPresentation = Character->GetHeldItemPresentationComponent())
	{
		HeldItemPresentation->RefreshPresentation();
	}
}
