#include "SAEHeldItemPresentationComponent.h"

#include "Inventory/SAEHandComponent.h"
#include "Player/SAEPlayerCharacter.h"
#include "GameFramework/SAEPlayerState.h"
#include "Inventory/SAEStolenEggComponent.h"
#include "World/Egg/SAEWorldEgg.h"

USAEHeldItemPresentationComponent::USAEHeldItemPresentationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//作用域：客户端&服务端
void USAEHeldItemPresentationComponent::RefreshPresentation()
{
	ClearPresentation();

	//DS服务器不创建视觉Actor
	if(GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	const ASAEPlayerCharacter* Character = Cast<ASAEPlayerCharacter>(GetOwner());
	const ASAEPlayerState* PlayerState = Character ? Character->GetPlayerState<ASAEPlayerState>() : nullptr;
	const USAEHandComponent* Hand = PlayerState ? PlayerState->GetHandComponent() : nullptr;
	if(!Hand)
	{
		return;
	}

	const FSAEHandState& HandState = Hand->GetHandStateRef();
	if(HandState.Source == ESAEHandItemSource::Quickbar)
	{
		ShowQuickbatItem(HandState);
	}
	else if(HandState.Source == ESAEHandItemSource::StolenEgg)
	{
		//StolenEgg 不生成本地Actor
		// ASAEWorldEgg 的服务器附加关系会复制到FrontCarryAnchor.
		ASAEWorldEgg* CarriedEgg = PlayerState->GetStolenEggComponent()->GetCarriedEgg();
		if(CarriedEgg)
		{
			CarriedEgg->AttachToComponent(Character->GetFrontCarryAnchor(),FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
}

//作用域：客户端&服务端
void USAEHeldItemPresentationComponent::ShowQuickbatItem(const FSAEHandState& HandeState)
{
	ASAEPlayerCharacter* Character = Cast<ASAEPlayerCharacter>(GetOwner());
	const FSAEHeldPresentationConfig* Config = ResolvePresentationConfig(HandeState.ItemDefinitionId);
	if(!Character || !Config || !Config->HeldActorClass)
	{
		return;
	}
	USceneComponent* AttachComponent = nullptr;
	FName SocketName = NAME_None;
	FTransform SlotTransform = FTransform::Identity;
	if(!Character->ResolveHeldItemSlot(Config->AttachmentSlotTag,AttachComponent,SocketName,SlotTransform))
	{
		return;
	}

	LocalHeldVisualActor = GetWorld()->SpawnActor<AActor>(Config->HeldActorClass, FTransform::Identity);
	if(!LocalHeldVisualActor)
	{
		return;
	}

	LocalHeldVisualActor->SetActorEnableCollision(false);
	LocalHeldVisualActor->AttachToComponent(AttachComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	LocalHeldVisualActor->SetActorRelativeTransform(Config->RelativeTransform * SlotTransform);
}

//作用域：客户端&服务端
void USAEHeldItemPresentationComponent::ClearPresentation()
{
	if(LocalHeldVisualActor)
	{
		LocalHeldVisualActor->Destroy();
		LocalHeldVisualActor = nullptr;
	}
}

//作用域：客户端&服务端
const FSAEHeldPresentationConfig* USAEHeldItemPresentationComponent::ResolvePresentationConfig(int32 DefinitionId) const
{
	return PresentationConfigs.Find(DefinitionId);
}

//作用域：客户端&服务端
void USAEHeldItemPresentationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearPresentation();
	Super::EndPlay(EndPlayReason);
}
