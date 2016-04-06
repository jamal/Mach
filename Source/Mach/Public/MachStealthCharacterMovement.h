// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachCharacterMovement.h"
#include "MachStealthCharacterMovement.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachStealthCharacterMovement : public UMachCharacterMovement
{
	GENERATED_UCLASS_BODY()
	float MaxWalkSpeedStealthed;
	float GrappleSpeed;
	FVector GrappleTarget;
	bool bFinishedGrappleMove;

	float GetMaxSpeed() const;

	void StartGrapple(FHitResult Impact);
	void StopGrapple();

	void PhysCustom(float deltaTime, int32 Iterations) override;
};
