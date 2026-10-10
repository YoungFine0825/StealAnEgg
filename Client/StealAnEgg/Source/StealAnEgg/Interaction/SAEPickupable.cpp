#include "SAEPickupable.h"

#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Player/SAEPlayerCharacter.h"

TScriptInterface<ISAEPickupable> USAEPickupableStatic::GetFirstPickupableFromActor(AActor* Actor)
{
	TScriptInterface<ISAEPickupable> Result;
	if(!Actor)
	{
		return Result;
	}
	if(Actor->GetClass()->ImplementsInterface(USAEPickupable::StaticClass()))
	{
		Result.SetObject(Actor);
		Result.SetInterface(Cast<ISAEPickupable>(Actor));
		return Result;
	}
	for(UActorComponent* Component : Actor->GetComponents())
	{
		if(Component && Component->GetClass()->ImplementsInterface(USAEPickupable::StaticClass()))
		{
			Result.SetObject(Component);
			Result.SetInterface(Cast<ISAEPickupable>(Component));
			return Result;
		}
	}
	return Result;
}

