#pragma once

#include "GameFramework/PlayerState.h"
#include "MachGameState.h"
#include "MachPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AMachPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()

	/** Array of players who will receive assist credit if this player dies */
	TArray<AMachPlayerState*>AssistPlayers;

	UPROPERTY(Transient, Replicated)
	uint32 bIsDead : 1;

	UPROPERTY(Transient, Replicated)
	TEnumAsByte<ETeam::Type> Team;

	TEnumAsByte<ETeam::Type> GetTeam();
	void SetTeam(ETeam::Type NewTeam);

	void SetIsDead(bool isDead);
	bool IsDead();

	void ScoreKill(AMachPlayerState* Victim);
	void ScoreDeath(AMachPlayerState* Killer);
	void ScoreAssist(AMachPlayerState* Victim);

	uint32 GetKills();
	uint32 GetDeaths();
	uint32 GetAssists();

	void AddAssistPlayer(AMachPlayerState* PlayerState);

protected:
	UPROPERTY(Transient, Replicated)
	uint32 NumKills;

	UPROPERTY(Transient, Replicated)
	uint32 NumDeaths;

	UPROPERTY(Transient, Replicated)
	uint32 NumAssists;
};
