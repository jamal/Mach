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

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = PlayerState)
	class AMachTechTree* TechTree;

	UPROPERTY(Transient, Replicated)
	uint32 bIsDead : 1;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = PlayerState)
	TEnumAsByte<ETeam::Type> Team;

	TEnumAsByte<ETeam::Type> GetTeam();
	void SetTeam(ETeam::Type NewTeam);

	void SetIsDead(bool isDead);
	bool IsDead();

	void ScoreKill(AMachPlayerState* Victim);
	void ScoreDeath(AMachPlayerState* Killer);
	void ScoreAssist(AMachPlayerState* Victim);
	
	void AddAssistPlayer(AMachPlayerState* PlayerState);

	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 NumKills;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 NumDeaths;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 NumAssists;
};
