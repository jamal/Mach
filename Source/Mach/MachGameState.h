#pragma once

#include "GameFramework/GameState.h"
#include "MachGameState.generated.h"

UENUM(BlueprintType)
namespace ETeam
{
	enum Type
	{
		/* Player joined the server but has not selected a team */
		None,
		A,
		B,
	};
}

/**
 * 
 */
UCLASS()
class AMachGameState : public AGameState
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Transient, Replicated)
	TEnumAsByte<ETeam::Type> WinningTeam;

	TArray<class APlayerState*> GetAllPlayers();
	TArray<class APlayerState*> GetPlayers(ETeam::Type Team);
};
