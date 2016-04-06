// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "MachGameState.h"
#include "MachTechTree.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachTechTree : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(Transient, Replicated)
	TEnumAsByte<ETeam::Type> Team;
	
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = TechTree)
	int32 Power;
};
