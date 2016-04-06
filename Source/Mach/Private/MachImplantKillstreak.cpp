// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachImplantKillstreak.h"


UMachImplantKillstreak::UMachImplantKillstreak()
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

	DamageBonusPerKill = .02;
	MaxDamageBonus = .2;

	LastKillStreak = 0;
}

void UMachImplantKillstreak::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Owner == nullptr || Owner->Role < ROLE_Authority)
	{
		return;
	}

	if (LastKillStreak != Owner->Killstreak)
	{
		// So we only update when the killstreak changes
		LastKillStreak = Owner->Killstreak;
		Owner->DamageBonusMultiplier = FMath::Min(Owner->Killstreak * DamageBonusPerKill, MaxDamageBonus);
		UE_LOG(LogTemp, Warning, TEXT("Updated the damage bonus to %.2f"), Owner->DamageBonusMultiplier);
	}
}

