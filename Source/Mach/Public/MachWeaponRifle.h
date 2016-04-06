// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachWeapon.h"
#include "MachWeaponRifle.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachWeaponRifle : public AMachWeapon
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rifle)
	bool bSniperEnabled;

	float AltDamageMultiplier;
	float AltTimeBetweenShotsMultiplier;
	float AltAmmoPerShot;

	void ConsumeAmmo() override;
	void StartSecondaryFire() override;
	
};
