#include "SAEWorldGameState.h"

#include "Net/UnrealNetwork.h"

void ASAEWorldGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASAEWorldGameState,WorldCycleState);
	DOREPLIFETIME(ASAEWorldGameState,ActiveRareTerritories);
}

float ASAEWorldGameState::GetRemainingPhaseTime() const
{
	const double Remaining = WorldCycleState.PhaseEndServerTime - GetServerWorldTimeSeconds();
	return static_cast<float>(Remaining);
}

void ASAEWorldGameState::SetWorldCycleState(const FSAEWorldCycleState& InNewState)
{
	if(!HasAuthority())
	{
		return;
	}
	WorldCycleState = InNewState;
	WorldCycleStateChangedDelegate.Broadcast(WorldCycleState);
}

void ASAEWorldGameState::SetActiveRareTerritories(const TArray<FSAEActiveRareTerritoryInfo>& NewRareTerritories)
{
	if(!HasAuthority())
	{
		return;
	}
	ActiveRareTerritories = NewRareTerritories;
	RareTerritoryStateChangedDelegate.Broadcast();
}

void ASAEWorldGameState::OnRep_WorldCycleState()
{
	WorldCycleStateChangedDelegate.Broadcast(WorldCycleState);
}

void ASAEWorldGameState::OnRep_ActiveRareTerritories()
{
	RareTerritoryStateChangedDelegate.Broadcast();
}
