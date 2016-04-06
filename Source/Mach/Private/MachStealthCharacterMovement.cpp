// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachStealthCharacterMovement.h"
#include "MachStealthCharacter.h"

UMachStealthCharacterMovement::UMachStealthCharacterMovement(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	MaxWalkSpeed = 950.f;
	MaxWalkSpeedCrouched = 380.f;
	SprintSpeed = 1250.f;
	GrappleSpeed = 1500.f;
	GrappleTarget = FVector::ZeroVector;

	bFinishedGrappleMove = false;

	MaxWalkSpeedStealthed = 475.f;
}

void UMachStealthCharacterMovement::StartGrapple(FHitResult Impact)
{
	// Don't allow player to grapple on to walkable surfaces
	if (!IsWalkable(Impact))
	{
		UE_LOG(LogTemp, Warning, TEXT("Start grapple"));
		Velocity = FVector::ZeroVector;
		bFinishedGrappleMove = false;
		GrappleTarget = Impact.ImpactPoint;
		SetMovementMode(MOVE_Custom);
	}
}

void UMachStealthCharacterMovement::StopGrapple()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop grapple"));
	SetMovementMode(MOVE_Falling);
}

float UMachStealthCharacterMovement::GetMaxSpeed() const
{
	AMachStealthCharacter* MachStealthCharacter = Cast<AMachStealthCharacter>(CharacterOwner);
	if (MachStealthCharacter && MachStealthCharacter->bIsStealthed)
	{
		return MaxWalkSpeedStealthed;
	}

	return Super::GetMaxSpeed();
}

void UMachStealthCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	UE_LOG(LogTemp, Warning, TEXT("Grapple move step"));

	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (bFinishedGrappleMove)
	{
		return;
	}

	FVector Location = CharacterOwner->GetActorLocation();
	const FVector Step = (GrappleTarget - Location).SafeNormal() * GrappleSpeed * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Step, CharacterOwner->GetActorRotation(), true, Hit);

	if (Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stopping grapple because we hit something"));
		bFinishedGrappleMove = true;
	}
}