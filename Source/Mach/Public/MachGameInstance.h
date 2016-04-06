// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "MachGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachGameInstance : public UGameInstance
{
	GENERATED_BODY()

	class UMachLocalPlayer* GetFirstMachPlayer();
	
};
