// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachHeavyCharacterMovement.h"
#include "MachHeavyCharacter.h"

UMachHeavyCharacterMovement::UMachHeavyCharacterMovement(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	MaxWalkSpeed = 850.f;
	MaxWalkSpeedCrouched = 340.f;

	SprintSpeed = 1150.0f;

	LeapHorizontalImpulse = 1500.f;
	LeapMaxHorizontalVelocity = 1500.f;
	LeapVerticalImpulse = 2000.f;
	LeapVelocityZReduction = 2000.f;

	ChargeMaxVelocity = 1000.f;
	ChargeStartVelocity = 1000.f;
	ChargeStepVelocity = 300.f;
	ChargeCurrentVelocity = 0.f;

	bIsLeaping = false;
}

void UMachHeavyCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	switch (CustomMovementMode)
	{
	case HEAVY_MOVEMENT_CUSTOM_Charging:
		PhysCharging(deltaTime, Iterations);
		break;

	}
}

bool UMachHeavyCharacterMovement::StartMovementSpecial(bool bReplayingMoves)
{
	if (CharacterOwner && MovementMode != MOVE_Custom)
	{
		ChargeCurrentVelocity = ChargeStartVelocity;
		SetMovementMode(MOVE_Custom, HEAVY_MOVEMENT_CUSTOM_Charging);
	}

	return true;
}

bool UMachHeavyCharacterMovement::StopMovementSpecial(bool bReplayingMoves)
{
	if (CharacterOwner && MovementMode == MOVE_Custom && CustomMovementMode == HEAVY_MOVEMENT_CUSTOM_Charging)
	{
		// Sudden stop
		Velocity = FVector::ZeroVector;
		SetMovementMode(MOVE_Walking);
	}

	return true;
}

bool UMachHeavyCharacterMovement::DoLeap(bool bReplayingMoves)
{
	if (CharacterOwner && !bIsMovementSpecialActive)
	{
		bIsMovementSpecialActive = true;
		bWasLeaping = true;
		bIsLeaping = true;
		LeapingTime = .75f;

		Velocity = CharacterOwner->GetActorRotation().Vector() * LeapHorizontalImpulse + FVector(0, 0, LeapVerticalImpulse);
		float SpeedCapped = FMath::Min(Velocity.Size(), LeapMaxHorizontalVelocity);
		Velocity = SpeedCapped * Velocity.SafeNormal();
		SetMovementMode(MOVE_Falling);
	}

	return true;
}

void UMachHeavyCharacterMovement::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	if (bWasLeaping)
	{
		bIsMovementSpecialActive = false;
		bIsLeaping = false;
		bWasLeaping = false;
		AMachHeavyCharacter* Character = Cast<AMachHeavyCharacter>(CharacterOwner);
		if (Character)
		{
			Character->LeapLanded(Hit);
		}
	}

	Super::ProcessLanded(Hit, remainingTime, Iterations);
}

void UMachHeavyCharacterMovement::PerformMovement(float DeltaTime)
{
	if (bWasLeaping)
	{
		/*
		LeapingTime -= DeltaTime;
		UE_LOG(LogTemp, Warning, TEXT("Leaping time %.2f"), LeapingTime);

		if (bIsLeaping && LeapingTime <= 0.1f)
		{
			bIsLeaping = false;
			Velocity = FVector(0, 0, 0);
			SetMovementMode(MOVE_Custom);
		}

		if (LeapingTime <= 0.f)
		{
			LeapingTime = 0.f;
			Velocity = FVector(0, 0, -2000.f);
			SetMovementMode(MOVE_Falling);
		}
		*/
		Velocity.Z -= 1000.f * DeltaTime;
	}

	Super::PerformMovement(DeltaTime);
}

void UMachHeavyCharacterMovement::PhysCharging(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// Adjust velocity to facing direction
	ChargeCurrentVelocity += ChargeStepVelocity * deltaTime;
	Velocity = FMath::Min(ChargeCurrentVelocity, ChargeMaxVelocity) * CharacterOwner->GetActorRotation().Vector().SafeNormal();

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
			StopMovementSpecial(false);
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
}