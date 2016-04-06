// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MachWeapon.h"
#include "MachWeaponCharge.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachWeaponCharge : public AMachWeapon
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	class UAnimationAsset* ChargeAnim;

	/** Max damage that will be dealt when fully charged */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float MaxDamage;
	
	/** How long in seconds to be fully charged */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float ChargeTime;

protected:
	UPROPERTY(Transient)
	class AMachProjectile* ChargeProjectile;

	void HandleFiring();
	void FireWeapon();
	void FireProjectile();
	
};
