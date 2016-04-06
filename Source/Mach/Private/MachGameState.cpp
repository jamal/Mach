#include "Mach.h"
#include "MachGameState.h"
#include "MachPlayerState.h"
#include "MachGameMode.h"

AMachGameState::AMachGameState(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	WinningTeam = ETeam::None;
}

TArray<APlayerState*> AMachGameState::GetAllPlayers()
{
	return PlayerArray;
}

TArray<APlayerState*> AMachGameState::GetPlayers(ETeam::Type Team)
{
		TArray<APlayerState*> TeamPlayers;
		for (APlayerState* Player : PlayerArray)
		{
			AMachPlayerState* CurPlayer = (AMachPlayerState*) Player;
			if (CurPlayer->Team == Team)
			{
			TeamPlayers.Add(Player);
		}
	}

	return TeamPlayers;
}

void AMachGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachGameState, WinningTeam);
}

bool AMachGameState::HasMatchStarted() const {
	if (MatchState == MatchState::PreGame)
	{
		return false;
	}

	return Super::HasMatchStarted();
}

bool AMachGameState::IsMatchInProgress() const {
	return Super::IsMatchInProgress();
}

bool AMachGameState::HasMatchEnded() const {
	return Super::HasMatchEnded();
}