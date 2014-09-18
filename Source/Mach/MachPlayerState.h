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

	UPROPERTY(Transient, Replicated)
	uint32 bIsDead : 1;

	UPROPERTY(Transient, Replicated)
	TEnumAsByte<ETeam::Type> Team;

	TEnumAsByte<ETeam::Type> GetTeam();
	void SetTeam(ETeam::Type NewTeam);

	void SetIsDead(bool isDead);
	bool IsDead();
};
