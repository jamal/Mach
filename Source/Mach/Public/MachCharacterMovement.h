// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "MachCharacterMovement.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float LimitedMovementModifier;

	/** Limit the character's movement when performing certain actions. */
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsMovementLimited;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	uint8 bIsSprinting : 1;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	uint8 bIsMovementSpecialActive : 1;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float SprintSpeed;

	UPROPERTY(Category="Character Movement", VisibleInstanceOnly, BlueprintReadOnly)
	uint32 bWantsToSprint : 1;

	float GetMaxSpeed() const;

	UFUNCTION(BlueprintCallable, Category = Movement)
	bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = Movement)
	virtual bool StartMovementSpecial(bool bReplayingMoves);

	UFUNCTION(BlueprintCallable, Category = Movement)
	virtual bool StopMovementSpecial(bool bReplayingMoves);
};

