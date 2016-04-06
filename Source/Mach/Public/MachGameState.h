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

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState)
	TEnumAsByte<ETeam::Type> WinningTeam;

	UFUNCTION(BlueprintCallable, Category=GameState)
	TArray<class APlayerState*> GetAllPlayers();

	UFUNCTION(BlueprintCallable, Category = GameState)
	TArray<class APlayerState*> GetPlayers(ETeam::Type Team);

	UFUNCTION(BlueprintCallable, Category=GameState)
	bool HasMatchStarted() const override;

	UFUNCTION(BlueprintCallable, Category = GameState)
	bool IsMatchInProgress() const override;

	UFUNCTION(BlueprintCallable, Category = GameState)
	bool HasMatchEnded() const override;

};
