// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "MachGameState.h"
#include "MachGameMode.generated.h"

namespace MatchState
{
	const FName PreGame = FName(TEXT("PreGame"));
}


UCLASS(minimalapi)
class AMachGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	int32 NumPlayersTeamA;

	UPROPERTY()
	int32 NumPlayersTeamB;

	FString InitNewPlayer(APlayerController* NewPlayerController, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal);

	virtual UClass* GetDefaultPawnClassForController(AController* InController) override;

	AActor* ChoosePlayerStart(AController* Player);

	bool IsMatchInProgress() const override;
	void HandleMatchHasStarted();
	void RespawnPlayers();

	bool IsFriendlyFireEnabled();
	void SetFriendlyFire(bool bEnabled);
	float GetFriendlyFireDamagePercent();

	/** Determine if two actors can damage each other based on the current game rules. */
	bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, const AActor* Causer, const AActor* Receiver) const;

	/** Returns how much damage should be dealt based on the actors and the game mode rules. For example,
	  * friendly fire could be enabled with a multiplier to reduce damage between friendly targets. The
	  * value returned by DamageAmount is the total damage that should be dealt. */
	float DamageAmount(float Damage, const AActor* Causer, const AActor* Receiver) const;

	// Tech point logic
	/** The tech point is no longer controlled by the team, decrease their power. */
	void UncaptureTechPoint(ETeam::Type OldTeam);
	void CaptureTechPoint(ETeam::Type Team);

	void GameOver(ETeam::Type WinningTeam);

	void Killed(AController* Killer, AController* Killed, class AMachCharacter* KilledCharacter);

	void PreInitializeComponents();

    /** Get the shield multiplier for a specific power level. */
    float ShieldMultiplierPower(int32 Power) const;
    /** Get the damage multiplier for a specific power level. */
    float DamageMultiplierPower(int32 Power) const;
    /** Get the current shield multiplier for a player. */
	float ShieldMultiplier(const AActor* Player) const;
    /** Get the current damage multiplier for a player. */
	float DamageMultiplier(const AActor* Causer) const;
    
    /** Performs actions such as updating player's shield when the team power changes. */
    void UpdateTeamPower(ETeam::Type Team, int32 Power);

	void Tick(float DeltaSeconds) override;
	bool PlayerCanRestart(APlayerController* Player) override;

	void SwapPawn(class AMachPlayerController* Controller);

protected:
	UPROPERTY()
	uint32 bRestartPostMatch : 1;

	UPROPERTY()
	uint32 bFriendlyFire : 1;

	/** How long to wait before starting a match after players join, set to 0 to disable. */
	UPROPERTY()
	float PregameTime;

	/** How long to wait before restarting a match after it ends. */
	UPROPERTY()
	float PostMatchTime;

	/** Time between respawn waves. */
	UPROPERTY()
	float RespawnWaveTime;

	/** Number of seconds before the last wave where we stop taking new dead players. */
	UPROPERTY()
	float RespawnWaveDeathLimit;

	/** Players that are in the current respawn wave. */
	TArray<AController*> RespawnWavePlayers;
	/** Players that didn't make it to the current respawn wave. */
	TArray<AController*> NextRespawnWavePlayers;

	float CurrentRespawnTimer;

	UPROPERTY()
	float FriendlyFireDamagePercent;

	float PregameStartTime;

	TSubclassOf<APawn> HeavyArmorPawnClass;
	TSubclassOf<APawn> MediumArmorPawnClass;
	TSubclassOf<APawn> LightArmorPawnClass;
	TSubclassOf<APawn> StealthArmorPawnClass;

	class AMachTechTree* TechTreeTeamA;
	class AMachTechTree* TechTreeTeamB;

	bool ReadyToStartMatch() override;
	bool ReadyToStartPreGame();

	void StartPreGame();
	void StartNewPlayer(APlayerController* NewPlayer);
	void Broadcast(AActor* Sender, const FString& Msg, FName Type = NAME_None) override;

	void UpdatePlayerVisibility();

public:
	/** Determine if damage between two actors is friendly. */
	// TODO: Rename this to not be specific to damage, because it isn't
	bool DamageIsFriendly(const AActor* Causer, const AActor* Receiver) const;
};
