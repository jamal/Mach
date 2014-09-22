// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "MachGameState.h"
#include "MachGameMode.generated.h"

UCLASS(minimalapi)
class AMachGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	int32 NumPlayersTeamA;

	UPROPERTY()
	int32 NumPlayersTeamB;

	APlayerController* Login(UPlayer* NewPlayer, const FString& Portal, const FString& Options, const TSharedPtr<FUniqueNetId>& UniqueId, FString& ErrorMessage);

	AActor* ChoosePlayerStart(AController* Player);

	void HandleMatchHasStarted();
	void RespawnTimer();

	bool IsFriendlyFireEnabled();
	void SetFriendlyFire(bool bEnabled);
	float GetFriendlyFireDamagePercent();

	/** Determine if two actors can damage each other based on the current game rules. */
	bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, const AActor* Causer, const AActor* Receiver) const;

	/** Returns how much damage should be dealt based on the actors and the game mode rules. For example,
	  * friendly fire could be enabled with a multiplier to reduce damage between friendly targets. The
	  * value returned by DamageAmount is the total damage that should be dealt. */
	float DamageAmount(float Damage, const AActor* Causer, const AActor* Receiver) const;

	void GameOver(ETeam::Type WinningTeam);

protected:
	UPROPERTY()
	uint32 bFriendlyFire : 1;

	UPROPERTY()
	float FriendlyFireDamagePercent;

private:
	/** Determine if damage between two actors is friendly. */
	bool DamageIsFriendly(const AActor* Causer, const AActor* Receiver) const;
};
