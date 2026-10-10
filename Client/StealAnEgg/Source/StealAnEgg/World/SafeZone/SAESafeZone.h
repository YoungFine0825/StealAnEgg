#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ActiveGameplayEffectHandle.h"
#include "SAESafeZone.generated.h"

class USAEGE_State_SafeZone;

UCLASS()
class STEALANEGG_API ASAESafeZone : public AActor
{
	GENERATED_BODY()
public:

	ASAESafeZone();

	virtual void BeginPlay() override;
	
protected:

	UPROPERTY(EditAnywhere,Category="SafeZone")
	TObjectPtr<UBoxComponent> SafeZoneBox;

	UPROPERTY(EditAnywhere,Category="SafeZone")
	TSubclassOf<USAEGE_State_SafeZone> SafeZoneEffectClass;

private:

	FActiveGameplayEffectHandle AddSafeZoneEffectHandle;

	UFUNCTION()
	void OnSafeZoneOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnSafeZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
