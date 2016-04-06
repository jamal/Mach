// Fill out your copyright notice in the Description page of Project Settings.

#include "Mach.h"
#include "MachCharacterMovement.h"


UMachCharacterMovement::UMachCharacterMovement(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	LimitedMovementModifier = 0.25f;
	bIsMovementLimited = false;
	bIsMovementSpecialActive = false;
	bIsSprinting = false;
	SprintSpeed = 800.f;
	GravityScale = 1.5f;

	JumpZVelocity = 700.f;

	NavAgentProps.bCanCrouch = true;
	NavAgentProps.bCanJump = true;
	NavAgentProps.bCanWalk = true;
	SetJumpAllowed(true);
	SetNetAddressable();
	SetIsReplicated(true);
}

bool UMachCharacterMovement::CanSprint() const
{
	if (CharacterOwner && IsMovingOnGround() && !IsCrouching())
	{
		// Can only sprint forward
		FRotator TurnRot(0.f, CharacterOwner->GetActorRotation().Yaw, 0.f);
		FVector X = FRotationMatrix(TurnRot).GetScaledAxis(EAxis::X);
		return ((X | Velocity.SafeNormal()) > 0.5f);
	}
	
	return false;
}

float UMachCharacterMovement::GetMaxSpeed() const
{
	float Speed = bIsSprinting && CanSprint() ? SprintSpeed : Super::GetMaxSpeed();
	if (bIsMovementLimited)
	{
		return Speed * LimitedMovementModifier;
	}

	return Speed;
}

bool UMachCharacterMovement::StartMovementSpecial(bool bReplayingMoves)
{
	return false;
}

bool UMachCharacterMovement::StopMovementSpecial(bool bReplayingMoves)
{
	return false;
}
