#include "SAEItemAppearanceActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ASAEItemAppearanceActor::ASAEItemAppearanceActor()
{
	bReplicates = false;
	PrimaryActorTick.bCanEverTick = false;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(DefaultRoot);
#if WITH_EDITORONLY_DATA
	DefaultRoot->bVisualizeComponent = true;
#endif
	

	StaticMesh0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh0"));
	StaticMesh0->SetupAttachment(DefaultRoot);
	StaticMesh0->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetActorEnableCollision(false);
}

