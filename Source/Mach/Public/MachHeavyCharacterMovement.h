// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachCharacterMovement.h"
#include "MachHeavyCharacterMovement.generated.h"

/** Custom movement modes for medium armor */
UENUM(BlueprintType)
enum EHeavyCustomMovementMode
{
	HEAVY_MOVEMENT_CUSTOM_Leaping		UMETA(DisplayName = "Leaping"),
	HEAVY_MOVEMENT_CUSTOM_Charging		UMETA(DisplayName = "Charging"),
};

/**
 * 
 */
UCLASS()
class MACH_API UMachHeavyCharacterMovement : public UMachCharacterMovement
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(EditDefaultsOnly, Category = MovementSpecial)
	float LeapHorizontalImpulse;

	UPROPERTY(EditDefaultsOnly, Category = MovementSpecial)
	float LeapVerticalImpulse;

	UPROPERTY(EditDefaultsOnly, Category = MovementSpecial)
	float LeapMaxHorizontalVelocity;
	
	UPROPERTY(EditDefaultsOnly, Category = MovementSpecial)
	float LeapVelocityZReduction;
	
	UPROPERTY(EditDefaultsOnly, Category = MovementSpecial)
	float ChargeStartVelocity;
	
	/** Velocity increase while charging per second. */
	UPROPERTY(EditDefaultsOnly, Category = MovementSpecial)
	float ChargeStepVelocity;
	
	UPROPERTY(EditDefaultsOnly, Category = MovementSpecial)
	float ChargeMaxVelocity;

	bool DoLeap(bool bReplayingMoves);
	bool StartMovementSpecial(bool bReplayingMoves);
	bool StopMovementSpecial(bool bReplayingMoves);
	void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations);
	void PerformMovement(float DeltaTime);
protected:
	float ChargeCurrentVelocity;
	float LeapingTime;
	bool bWasLeaping;
	bool bIsLeaping;

	void PhysCustom(float deltaTime, int32 Iterations);
	//void PhysLeaping(float deltaTime, int32 Iterations);
	void PhysCharging(float deltaTime, int32 Iterations);
};
