#pragma once

#include "CoreMinimal.h"
#include "Interaction/SAEInteractionOption.h"
#include "Interaction/SAEPickupable.h"
#include "SAEInteractionMessages.generated.h"

USTRUCT(BlueprintType)
struct STEALANEGG_API FSAEInteractionOptionsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FSAEInteractionOption> Options;

	UPROPERTY(BlueprintReadOnly)
	int32 SelectedIndex;
};

USTRUCT(BlueprintType)
struct STEALANEGG_API FSAEPickupHoldMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bIsHolding = false;

	UPROPERTY(BlueprintReadOnly)
	float Duration = 0;

	UPROPERTY(BlueprintReadOnly)
	FSAEPickupInfo PickupInfo;
};