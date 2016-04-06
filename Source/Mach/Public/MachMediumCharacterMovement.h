// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachCharacterMovement.h"
#include "MachMediumCharacterMovement.generated.h"

/** Custom movement modes for medium armor */
UENUM(BlueprintType)
enum EMediumCustomMovementMode
{
	/** Directional dash. */
	MEDIUM_MOVEMENT_CUSTOM_Dashing		UMETA(DisplayName = "Dashing"),

	/** Engage thrusters while jumping to hover. */
	MEDIUM_MOVEMENT_CUSTOM_Hovering		UMETA(DisplayName = "Hovering"),
};

/**
 * 
 */
UCLASS()
class MACH_API UMachMediumCharacterMovement : public UMachCharacterMovement
{
	GENERATED_UCLASS_BODY()
	
	float DashImpulse;
	
	float DashImpulseMaxSpeed;
	float MovementSpecialTime;
	bool StartMovementSpecial(bool bReplayingMoves);

	void StartHovering();
	void StopHovering();

protected:
	FVector HoveringVelocity;
	float HoverMaxSpeed;
	FVector MS_StartLocation;
	float DashMaxDistance;
	void PhysCustom(float deltaTime, int32 Iterations);
	void PhysDashing(float deltaTime, int32 Iterations);
	void PhysHovering(float deltaTime, int32 Iterations);
};
