#include "SAESafeZone.h"

#include "Player/SAEPlayerCharacter.h"
#include "GameFramework/SAEPlayerState.h"
#include "AbilitySystem/SAEAbilitySystemComponent.h"
#include "AbilitySystem/SAEGameplayTags.h"
#include "AbilitySystem/GameEffects/SAEGE_State_SafeZone.h"
#include "Inventory/SAEStolenEggComponent.h"

ASAESafeZone::ASAESafeZone()
{
	SafeZoneBox = CreateDefaultSubobject<UBoxComponent>("SafeZoneBox");
	SetRootComponent(SafeZoneBox);
	
}

void ASAESafeZone::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority() && IsValid(SafeZoneBox))
	{
		SafeZoneBox->OnComponentBeginOverlap.AddDynamic(this,&ASAESafeZone::OnSafeZoneOverlapBegin);
		SafeZoneBox->OnComponentEndOverlap.AddDynamic(this,&ASAESafeZone::OnSafeZoneOverlapEnd);
	}
}

void ASAESafeZone::OnSafeZoneOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if(!HasAuthority())
	{
		return;
	}

	ASAEPlayerCharacter* SAEPlayerCharacter = Cast<ASAEPlayerCharacter>(OtherActor);
	if(!IsValid(SAEPlayerCharacter))
	{
		return;
	}

	UAbilitySystemComponent* ASC = SAEPlayerCharacter->GetAbilitySystemComponent();
	if(!IsValid(ASC))
	{
		return;
	}
	
	if(IsValid(SafeZoneEffectClass))
	{
		const UGameplayEffect* GameplayEffectCDO = SafeZoneEffectClass->GetDefaultObject<USAEGE_State_SafeZone>();
		AddSafeZoneEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffectCDO,0,ASC->MakeEffectContext());
	}

	//调用USAEStolenEggComponent组件结算偷的蛋
	ASAEPlayerState* SAEPlayerState = SAEPlayerCharacter->GetPlayerState<ASAEPlayerState>();
	if(IsValid(SAEPlayerState))
	{
		USAEStolenEggComponent* StolenEggComponent = SAEPlayerState->GetStolenEggComponent();
		if(IsValid(StolenEggComponent) && StolenEggComponent->IsCarriedEgg())
		{
			StolenEggComponent->CommitStolenEgg();
		}
	}
}


void ASAESafeZone::OnSafeZoneOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if(!HasAuthority())
	{
		return;
	}

	ASAEPlayerCharacter* SAEPlayerCharacter = Cast<ASAEPlayerCharacter>(OtherActor);
	if(!IsValid(SAEPlayerCharacter))
	{
		return;
	}

	UAbilitySystemComponent* ASC = SAEPlayerCharacter->GetAbilitySystemComponent();
	if(!IsValid(ASC))
	{
		return;
	}

	ASC->RemoveActiveGameplayEffect(AddSafeZoneEffectHandle);
}
