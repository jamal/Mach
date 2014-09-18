// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

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

protected:
	UPROPERTY()
	uint32 bFriendlyFire : 1;

	UPROPERTY()
	float FriendlyFireDamagePercent;

};
