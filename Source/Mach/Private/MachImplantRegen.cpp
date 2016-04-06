// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachImplantRegen.h"

UMachImplantRegen::UMachImplantRegen()
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

	TimeOutOfCombat = 5.0f;
	HealthPerTick = 3.0f;
	TickTime = 4.0f;
}

void UMachImplantRegen::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Owner == nullptr)
	{
		return;
	}

	if (Owner->TimeSinceLastDamageTaken >= TimeOutOfCombat && Owner->Health < Owner->BaseHealth)
	{
		bActive = true;
		TickTimer += DeltaTime;

		if (TickTimer >= TickTime)
		{
			TickTimer -= TickTime;
			Owner->Health += HealthPerTick;
		}
	}
	else
	{
		if (bActive)
		{
			TickTimer = 0.f;
		}

		bActive = false;
	}
}

