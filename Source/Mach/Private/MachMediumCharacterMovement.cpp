// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachMediumCharacterMovement.h"


UMachMediumCharacterMovement::UMachMediumCharacterMovement(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	MaxWalkSpeed = 950.f;
	MaxWalkSpeedCrouched = 380.f;
	SprintSpeed = 1250.f;

	DashImpulse = 3000.f;
	DashImpulseMaxSpeed = 3000.f;
	DashMaxDistance = 750.f;
	HoverMaxSpeed = 3000.f;
}

bool UMachMediumCharacterMovement::StartMovementSpecial(bool bReplayingMoves)
{
	if (CharacterOwner && MovementMode != MOVE_Custom)
	{
		MS_StartLocation = CharacterOwner->GetActorLocation();
		MovementSpecialTime = .2f;
		FVector VelocityNormal = Velocity.SafeNormal();
		if (VelocityNormal.Size() < 0.5f)
		{
			// Default to dash backwards
			UE_LOG(LogTemp, Warning, TEXT("Velocity %.2f"), VelocityNormal.Size());
			VelocityNormal = CharacterOwner->GetActorRotation().Vector().SafeNormal() * -1;
		}

		Velocity = VelocityNormal * DashImpulse;
		float SpeedCapped = FMath::Min(Velocity.Size(), DashImpulseMaxSpeed);
		Velocity = SpeedCapped * Velocity.SafeNormal();
		SetMovementMode(MOVE_Custom, MEDIUM_MOVEMENT_CUSTOM_Dashing);
	}

	return true;
}

void UMachMediumCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	switch (CustomMovementMode)
	{
	case MEDIUM_MOVEMENT_CUSTOM_Dashing:
		PhysDashing(deltaTime, Iterations);
		break;

	case MEDIUM_MOVEMENT_CUSTOM_Hovering:
		PhysHovering(deltaTime, Iterations);
		break;

	}
}

void UMachMediumCharacterMovement::PhysDashing(float deltaTime, int32 Iterations)
{
	if (!HasRootMotion())
	{
		if (bCheatFlying && Acceleration.IsZero())
		{
			Velocity = FVector::ZeroVector;
		}
		const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction;
		CalcVelocity(deltaTime, Friction, true, BrakingDecelerationFlying);
	}

	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = CharacterOwner->GetActorLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, CharacterOwner->GetActorRotation(), true, Hit);

	if (Hit.Time < 1.f && CharacterOwner)
	{
		const FVector GravDir = FVector(0.f, 0.f, -1.f);
		const FVector VelDir = Velocity.SafeNormal();
		const float UpDown = GravDir | VelDir;

		bool bSteppedUp = false;

		if (!IsWalkable(Hit))
		{
			// We hit a wall
			SetMovementMode(MOVE_Walking, 0);
			return;
		}
		else if ((FMath::Abs(Hit.ImpactNormal.Z) < 0.2f) && (UpDown < 0.5f) && (UpDown > -0.2f) && CanStepUp(Hit))
		{
			float stepZ = CharacterOwner->GetActorLocation().Z;
			bSteppedUp = StepUp(GravDir, Adjusted * (1.f - Hit.Time), Hit);
			if (bSteppedUp)
			{
				OldLocation.Z = CharacterOwner->GetActorLocation().Z + (OldLocation.Z - stepZ);
			}
		}

		if (!bSteppedUp)
		{
			//adjust and try again
			HandleImpact(Hit, deltaTime, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}
	}

	if (!bJustTeleported && !HasRootMotion() && CharacterOwner)
	{
		// Update the velocity every frame so we can use slope to gain Z velocity
		Velocity = (CharacterOwner->GetActorLocation() - OldLocation) / deltaTime;
	}

	const FVector TravelDistance = MS_StartLocation - CharacterOwner->GetActorLocation();
	if (TravelDistance.Size() >= DashMaxDistance)
	{
		SetMovementMode(MOVE_Walking, 0);
	}
}

void UMachMediumCharacterMovement::PhysHovering(float deltaTime, int32 Iterations)
{
	if (!HasRootMotion())
	{
		if (bCheatFlying && Acceleration.IsZero())
		{
			Velocity = FVector::ZeroVector;
		}
		const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction;
		CalcVelocity(deltaTime, Friction, true, BrakingDecelerationFlying);
	}

	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = CharacterOwner->GetActorLocation();

	FVector Adjusted = Velocity;
	Adjusted.Z = 15.f;

	float SpeedCapped = FMath::Min(Adjusted.Size(), HoverMaxSpeed);
	Adjusted = SpeedCapped * Adjusted.SafeNormal();
	Adjusted = Adjusted * deltaTime;

	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, CharacterOwner->GetActorRotation(), true, Hit);

	if (Hit.Time < 1.f && CharacterOwner)
	{
		const FVector GravDir = FVector(0.f, 0.f, -1.f);
		const FVector VelDir = Velocity.SafeNormal();
		const float UpDown = GravDir | VelDir;

		bool bSteppedUp = false;
		if ((FMath::Abs(Hit.ImpactNormal.Z) < 0.2f) && (UpDown < 0.5f) && (UpDown > -0.2f) && CanStepUp(Hit))
		{
			float stepZ = CharacterOwner->GetActorLocation().Z;
			bSteppedUp = StepUp(GravDir, Adjusted * (1.f - Hit.Time), Hit);
			if (bSteppedUp)
			{
				OldLocation.Z = CharacterOwner->GetActorLocation().Z + (OldLocation.Z - stepZ);
			}
		}

		if (!bSteppedUp)
		{
			//adjust and try again
			HandleImpact(Hit, deltaTime, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}
	}

	if (!bJustTeleported && !HasRootMotion() && CharacterOwner)
	{
		Velocity = (CharacterOwner->GetActorLocation() - OldLocation) / deltaTime;
	}

	MovementSpecialTime -= deltaTime;
	if (MovementSpecialTime <= 0.f)
	{
		StopHovering();
	}
}

void UMachMediumCharacterMovement::StartHovering()
{
	if (MovementMode == MOVE_Falling)
	{
		MovementSpecialTime = 3.0f;
		SetMovementMode(MOVE_Custom, MEDIUM_MOVEMENT_CUSTOM_Hovering);
	}
}

void UMachMediumCharacterMovement::StopHovering()
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == MEDIUM_MOVEMENT_CUSTOM_Hovering)
	{
		SetMovementMode(MOVE_Falling, 0);
	}
}
