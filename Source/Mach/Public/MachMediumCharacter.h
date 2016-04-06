// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachCharacter.h"
#include "MachMediumCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachMediumCharacter : public AMachCharacter
{
	GENERATED_UCLASS_BODY()

	bool bIsHovering;

	virtual void PostInitializeComponents() override;

protected:

	class UMachMediumCharacterMovement* MediumMovement;
	bool bJustHovered;
	bool bHoverQueued;
	void StartJump();
	void StopJump();
	void Tick(float DeltaSeconds);
	void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0);
	bool CanDoMovementSpecial();
	bool CanHover();
};
