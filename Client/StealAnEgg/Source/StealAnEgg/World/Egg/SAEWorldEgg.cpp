#include "SAEWorldEgg.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/SAEGameplayTags.h"
#include "GameFramework/SAEPlayerState.h"
#include "Inventory/SAEHandComponent.h"
#include "Inventory/SAEQuickbarComponent.h"
#include "Inventory/SAEStolenEggComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/SAEPlayerCharacter.h"
#include "Components/ChildActorComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystem/Abilities/SAEGA_Pickup.h"


ASAEWorldEgg::ASAEWorldEgg()
{
	bReplicates = true;

	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
#endif

	AppearenceComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Appearence"));
	AppearenceComponent->SetupAttachment(RootComponent);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);
}

void ASAEWorldEgg::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		RefreshPresentation();
		
		if(ItemDefinitionId > 0)
		{
			//ItemDefinitionId以赋值时，初始化获取EggId,然后推送给客户端
			if(ResolveEggConfigByBlueprint())
			{
				ForceNetUpdate();
			}
			if(CanCreateEggAppearance())
			{
				CreateEggAppearance(CurrentEggDefinitionId);
			}
		}
	}
}

bool ASAEWorldEgg::ResetItemDefinitionId(int32 InItemDefinitionId)
{
	if(!HasAuthority())
	{
		return false;
	}
	if(InItemDefinitionId == ItemDefinitionId)
	{
		return false;
	}
	ItemDefinitionId = InItemDefinitionId;
	ResolveEggConfigByBlueprint();
	//
	if(CanCreateEggAppearance())
	{
		CreateEggAppearance(CurrentEggDefinitionId);
	}
	//
	ForceNetUpdate();
	return true;
}

void ASAEWorldEgg::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASAEWorldEgg,EggState);
	DOREPLIFETIME(ASAEWorldEgg,CarryingPlayer);
	DOREPLIFETIME(ASAEWorldEgg,ItemDefinitionId);
	DOREPLIFETIME(ASAEWorldEgg,CurrentEggDefinitionId);
}

void ASAEWorldEgg::OnRep_ItemDefinitionId()
{
	
}

void ASAEWorldEgg::OnRep_CurrentEggDefinitionId()
{
	if(CurrentEggDefinitionId != INDEX_NONE && CanCreateEggAppearance())
	{
		CreateEggAppearance(CurrentEggDefinitionId);
	}
}

bool ASAEWorldEgg::CanCreateEggAppearance() const
{
	bool Ret = GetNetMode() != NM_DedicatedServer;
	return Ret;
}


void ASAEWorldEgg::CreateEggAppearance_Implementation(int32 EggDefinitionId)
{
	
}


bool ASAEWorldEgg::CanInteract(const ASAEPlayerCharacter* Character, FText& OutFailureReason) const
{
	if(!IsAvailable() || !Character)
	{
		OutFailureReason = FText::FromString(TEXT("不可拾取"));
		return false;
	}

	if(FVector::DistSquared(GetActorLocation(),Character->GetActorLocation()) > FMath::Square(MaximumPickupDistance))
	{
		OutFailureReason = FText::FromString(TEXT("距离过远"));
		return false;
	}
	const UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if(!ASC
		|| ASC->HasMatchingGameplayTag(SAEGameplayTags::State_Zone_Safe)
		|| ASC->HasMatchingGameplayTag(SAEGameplayTags::State_World_Refreshing))
	{
		OutFailureReason = FText::FromString(TEXT("当前状态不允许拾取"));
		return false;
	}
	const ASAEPlayerState* PlayerState = Character->GetPlayerState<ASAEPlayerState>();
	if(!PlayerState)
	{
		OutFailureReason = FText::FromString(TEXT("当前角色不允许拾取"));
		return false;
	}
	if(PlayerState->GetHandComponent()->IsCarringStolenEgg())
	{
		OutFailureReason = FText::FromString(TEXT("只能拾取一个蛋"));
		return false;
	}
	if(!PlayerState->GetQuickbarComponent()->HasFreeSlot())
	{
		OutFailureReason = FText::FromString(TEXT("没有空位"));
		return false;
	}
	return true;
}

FSAEPickupInfo ASAEWorldEgg::GetPickupInfo() const
{
	FSAEPickupInfo Info;
	Info.ItemDefinitionId = ItemDefinitionId;
	Info.Quantity = 1;
	Info.PickupTags.AddTag(SAEGameplayTags::Pickup_StolenEgg);
	return Info;
}

bool ASAEWorldEgg::CanBePickedUpBy(const ASAEPlayerCharacter* Character, FText& OutFailureReason) const
{
	return CanInteract(Character, OutFailureReason);
}

bool ASAEWorldEgg::CommitPickup(ASAEPlayerCharacter* Character, FText& OutFailureReason)
{
	if(!HasAuthority())
	{
		OutFailureReason = NSLOCTEXT("Pickup", "ServerOnly","拾取只能由服务器执行");
		return false;
	}
	if(!CanBePickedUpBy((ASAEPlayerCharacter*)Character, OutFailureReason))
	{
		return false;
	}
	ASAEPlayerState* PlayerState = Character ? Character->GetPlayerState<ASAEPlayerState>() : nullptr;
	USAEStolenEggComponent* StolenEggComponent = PlayerState->GetStolenEggComponent();
	if(!StolenEggComponent || !StolenEggComponent->TryCarryEgg(this))
	{
		OutFailureReason = NSLOCTEXT("Pickup", "CannotCarryEgg","无法携带这个蛋");
		return false;
	}

	EggState = ESAEWorldEggState::Carried;
	CarryingPlayer = PlayerState;
	RefreshPresentation();
	ForceNetUpdate();
	return true;
}

void ASAEWorldEgg::GatherInteractionOptions(const ASAEPlayerCharacter* Character, FSAEInteractionOptionBuilder& Builder) const
{
	if(!PickupAbilityClass)
	{
		return;
	}
	FSAEInteractionOption Option;
	Option.AbilityClass = PickupAbilityClass;
	Option.InteractionTag = SAEGameplayTags::Interaction_Pickup_StolenEgg;
	Option.InteractionStyleTag = SAEGameplayTags::UI_Interaction_Hold;
	Option.Duration = FMath::Max(0.0f,PickDuration);
	Option.Text = NSLOCTEXT("Interaction","PickupStolenEgg","拾取");
	Option.SubText = NSLOCTEXT("Interaction","HoldToPickupStolenEgg","按住以拾取");
	Builder.AddOption(Option);
}

bool ASAEWorldEgg::TryPickup(ASAEPlayerCharacter* Character)
{
	FText R;
	if(!HasAuthority() || !CanInteract(Character,R))
	{
		return false;
	}
	ASAEPlayerState* PlayerState = Character->GetPlayerState<ASAEPlayerState>();
	if(!PlayerState->GetStolenEggComponent()->TryCarryEgg(this))
	{
		return false;
	}
	EggState = ESAEWorldEggState::Carried;
	CarryingPlayer = PlayerState;
	RefreshPresentation();
	ForceNetUpdate();
	return true;
}

void ASAEWorldEgg::DropAt(const FVector& Location)
{
	if(!HasAuthority())
	{
		return;
	}
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	EggState = ESAEWorldEggState::Dropped;
	CarryingPlayer = nullptr;
	SetActorLocation(Location);
	RefreshPresentation();
}

//蛋被拾取成功，转换为道具进入玩家背包
void ASAEWorldEgg::ConsumeAfterSettlement()
{
	if(HasAuthority())
	{
		EggState = ESAEWorldEggState::Settled;
		Destroy();
	}
}

void ASAEWorldEgg::OnRep_EggState()
{
	RefreshPresentation();
}

void ASAEWorldEgg::RefreshPresentation()
{
	bool W = EggState == ESAEWorldEggState::Available || EggState == ESAEWorldEggState::Dropped;
	CollisionComponent->SetCollisionEnabled(W ? ECollisionEnabled::QueryAndPhysics
								   : ECollisionEnabled::NoCollision);
}
