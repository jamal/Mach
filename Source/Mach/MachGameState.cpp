

#include "Mach.h"
#include "MachGameState.h"
#include "GameFramework/PlayerState.h"
#include "MachPlayerState.h"

AMachGameState::AMachGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

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
