// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachMediumCharacter.h"
#include "MachMediumCharacterMovement.h"

AMachMediumCharacter::AMachMediumCharacter(const class FObjectInitializer& PCIP)
	: Super(PCIP.SetDefaultSubobjectClass<UMachMediumCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	BaseHealth = 150.0f;
	BaseShield = 75.0f;
	ShieldRegenStartTime = 5.0f;
	ShieldRegenRate = 20.0f;
	MovementSpecialCost = 25;
	
	bIsHovering = false;
	bHoverQueued = false;


	UE_LOG(LogTemp, Warning, TEXT("Instansiating medium armor"));
}

void AMachMediumCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MediumMovement = Cast<UMachMediumCharacterMovement>(MachCharacterMovement);
}

void AMachMediumCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (bIsHovering && CharacterMovement->CustomMovementMode != MEDIUM_MOVEMENT_CUSTOM_Hovering)
	{
		bIsHovering = false;
	}
}

void AMachMediumCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bHoverQueued && CanHover())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hovering from queue"));
		bHoverQueued = false;
		StartJump();
	}

	if (bIsHovering)
	{
		Energy -= 5.f * DeltaSeconds;
	}
}

bool AMachMediumCharacter::CanHover()
{
	return MachCharacterMovement->MovementMode == MOVE_Falling;
}

void AMachMediumCharacter::StartJump()
{
	// If we're about to do something custom, make sure we do it on the server too
	if (Role < ROLE_Authority && (CanHover() || MediumMovement->MovementMode == MOVE_Custom))
	{
		ServerOnStartJump(GetLastMovementInputVector());
	}

	if (CanHover())
	{
		bIsHovering = true;
		bSpendingEnergy = true;
		MediumMovement->StartHovering();
	}
	else if (MediumMovement->MovementMode == MOVE_Custom)
	{
		// If we're dashing, queue up the hover
		bHoverQueued = true;
	}
	else
	{
		Super::StartJump();
	}
}

void AMachMediumCharacter::StopJump()
{
	if (Role < ROLE_Authority && MediumMovement->MovementMode == MOVE_Custom)
	{
		ServerOnStopJump();
	}

	bHoverQueued = false;
	if (MediumMovement->MovementMode == MOVE_Custom && MachCharacterMovement->CustomMovementMode == MEDIUM_MOVEMENT_CUSTOM_Hovering)
	{
		bIsHovering = false;
		bSpendingEnergy = false;
		MediumMovement->StopHovering();
	}

	Super::StopJump();
}

bool AMachMediumCharacter::CanDoMovementSpecial()
{
	return MachCharacterMovement->CustomMovementMode != MEDIUM_MOVEMENT_CUSTOM_Hovering;
}
