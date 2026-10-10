#include "SAEInteractionComponent.h"

#include "GameFramework/Pawn.h"
#include "Interaction/SAEInteractableTarget.h"

USAEInteractionComponent::USAEInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

AActor* USAEInteractionComponent::FindBestInteractable() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if(!Pawn || !Pawn->GetController() || !GetWorld())
	{
		return nullptr;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	Pawn->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SAEInteraction),false,Pawn);
	const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * InteractionRange;
	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		ViewLocation,
		TraceEnd,
		FQuat::Identity,TraceChannel,
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams);
	AActor* Target = bHit ? Hit.GetActor() : nullptr;
	if(!Target || !Target->Implements<USAEInteractableTarget>())
	{
		return nullptr;
	}
	return Target;
}

