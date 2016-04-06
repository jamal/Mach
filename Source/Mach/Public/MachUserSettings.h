// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "GameFramework/SaveGame.h"
#include "MachUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachUserSettings : public USaveGame
{
	GENERATED_BODY()

public:

	
private:
	bool bIsDirty;

	FString Name;
};
