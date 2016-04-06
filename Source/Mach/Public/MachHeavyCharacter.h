// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MachCharacter.h"
#include "MachHeavyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachHeavyCharacter : public AMachCharacter
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, Category=Heavy)
	class UCapsuleComponent* ChargeTrigger;

	float LeapHitVerticalImuplse;
	float LeapHitHorizontalImpulse;
	float ChargeHitVerticalImuplse;
	float ChargeHitHorizontalImpulse;
	float LastJumpTime;
	float LeapCost;
	float ChargeTurnRateModifier;
	void LeapLanded(const FHitResult& HitResult);
	void LeapEnded();
	void StartMovementSpecial() override;
	void StartJump() override;
	void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnChargeTriggerOverlap(class AActor * OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void OnStartFire() override;
	void OnReload() override;
	
};
