// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachImplantComponent.h"
#include "MachImplantRegen.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachImplantRegen : public UMachImplantComponent
{
	GENERATED_BODY()
	
public:
	UMachImplantRegen();

	/** If the implant is currently healing. */
	bool bActive;
	/** How long after combat before we start regen. */
	float TimeOutOfCombat;
	/** Health added per tick. */
	float HealthPerTick;
	/** Seconds per tick. */
	float TickTime;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	float TickTimer;
	
};
