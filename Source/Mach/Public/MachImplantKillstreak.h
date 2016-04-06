// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachImplantComponent.h"
#include "MachImplantKillstreak.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachImplantKillstreak : public UMachImplantComponent
{
	GENERATED_BODY()

public:
	UMachImplantKillstreak();
	
	float DamageBonusPerKill;
	float MaxDamageBonus;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	int32 LastKillStreak;
};
