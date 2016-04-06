// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachCharacterMovement.h"
#include "MachLightCharacterMovement.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachLightCharacterMovement : public UMachCharacterMovement
{
	GENERATED_UCLASS_BODY()

	float JumpTurnVelocity;
	float BlinkImpulse;
	float BlinkMaxSpeed;
	float MovementSpecialTime;
	FVector MS_StartLocation;
	float BlinkMaxDistance;
	FVector OldVelocity;
	bool StartMovementSpecial(bool bReplayingMoves);
	void PhysCustom(float deltaTime, int32 Iterations);

	virtual bool DoJump(bool bReplayingMoves) override;
};
