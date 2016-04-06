// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachLightCharacterMovement.h"
#include "MachLightCharacter.h"

UMachLightCharacterMovement::UMachLightCharacterMovement(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	MaxWalkSpeed = 1050.f;
	MaxWalkSpeedCrouched = 420.f;
	SprintSpeed = 1350.f;

	BlinkImpulse = 10000.f;
	BlinkMaxSpeed = 10000.f;
	BlinkMaxDistance = 1050.f;

	JumpTurnVelocity = 600.f;
}

bool UMachLightCharacterMovement::DoJump(bool bReplayingMoves)
{
	AMachLightCharacter* LightCharacter = Cast<AMachLightCharacter>(GetCharacterOwner());
	if (LightCharacter)
	{
		FVector MovementInput;
		// On the server we can't read GetLastInputVector, so we pass that in on the jump call
		if (CharacterOwner->Role == ROLE_Authority)
		{
			MovementInput = LightCharacter->ServerJumpMovementInput;
		}
		else
		{
			MovementInput = GetLastInputVector();
		}

		if (CharacterOwner && CharacterOwner->CanJump())
		{
			// Don't jump if we can't move up/down.
			if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
			{
				Velocity.Z = JumpZVelocity;

				if (LightCharacter->Jumps > 0 && !MovementInput.IsNearlyZero())
				{
					if (Velocity.X < JumpTurnVelocity)
					{
						Velocity.X = MovementInput.X * JumpTurnVelocity;
					}

					if (Velocity.Y < JumpTurnVelocity)
					{
						Velocity.Y = MovementInput.Y * JumpTurnVelocity;
					}
				}

				SetMovementMode(MOVE_Falling);

				LightCharacter->Jumps++;

				return true;
			}
		}
	}

	return false;
}

bool UMachLightCharacterMovement::StartMovementSpecial(bool bReplayingMoves)
{
	if (CharacterOwner && MovementMode != MOVE_Custom)
	{
		OldVelocity = Velocity;

		MS_StartLocation = CharacterOwner->GetActorLocation();
		MovementSpecialTime = .3f;

		FVector VelDir = CharacterOwner->GetActorRotation().Vector().SafeNormal();
		Velocity = VelDir * BlinkImpulse;
		float SpeedCapped = FMath::Min(Velocity.Size(), BlinkMaxSpeed);
		Velocity = SpeedCapped * Velocity.SafeNormal();

		SetMovementMode(MOVE_Custom);
	}

	return true;
}

void UMachLightCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
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

		UE_LOG(LogTemp, Warning, TEXT("Hit ImpactNormal (%.2f, %.2f, %.2f)"), Hit.ImpactNormal.X, Hit.ImpactNormal.Y, Hit.ImpactNormal.Z);

		bool bSteppedUp = false;
		if (!IsWalkable(Hit))
		{
			// We hit a wall
			Velocity = Velocity.SafeNormal() * OldVelocity.Size();
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

	const FVector TravelDistance = MS_StartLocation - CharacterOwner->GetActorLocation();
	if (TravelDistance.Size() >= BlinkMaxDistance)
	{
		Velocity = Velocity.SafeNormal() * OldVelocity.Size();
		SetMovementMode(MOVE_Walking, 0);
	}
}
