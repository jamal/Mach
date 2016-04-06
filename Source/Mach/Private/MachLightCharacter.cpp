// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachLightCharacter.h"
#include "MachLightCharacterMovement.h"

AMachLightCharacter::AMachLightCharacter(const class FObjectInitializer& PCIP)
	: Super(PCIP.SetDefaultSubobjectClass<UMachLightCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	BaseHealth = 100.0f;
	BaseShield = 50.0f;
	ShieldRegenStartTime = 5.0f;
	ShieldRegenRate = 30.0f;
	MovementSpecialCost = 30.f;

	Jumps = 0;
}

bool AMachLightCharacter::CanJumpInternal_Implementation() const
{
	if (Jumps < 2)
	{
		return true;
	}

	return Super::CanJumpInternal_Implementation();
}

void AMachLightCharacter::OnLanded(const FHitResult& Hit)
{
	Jumps = 0;
}
