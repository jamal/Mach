// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MachProjectile.h"
#include "MachChargeProjectile.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachChargeProjectile : public AMachProjectile
{
	GENERATED_UCLASS_BODY()

	void OnCollisionOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
