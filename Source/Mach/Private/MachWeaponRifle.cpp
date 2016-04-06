// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachWeaponRifle.h"
#include "MachCharacter.h"

AMachWeaponRifle::AMachWeaponRifle(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bSniperEnabled = false;

	AltDamageMultiplier = 6.f;
	AltTimeBetweenShotsMultiplier = 40.f;
	AltAmmoPerShot = 10.f;

	Damage = 15.f;
	MaxAmmo = 250.f;
	Ammo = 50.f;
	TimeBetweenShots = 0.075f;
	BurstMode = EWeaponBurstMode::Full;
	Resource = EWeaponResource::Ammo;
	Range = 12000.f;
	WeaponModelType = EWeaponModelType::Rifle;
}

void AMachWeaponRifle::ConsumeAmmo()
{
	if (bSniperEnabled)
	{
		Ammo -= AltAmmoPerShot;
	}
	else
	{
		Super::ConsumeAmmo();
	}
}

void AMachWeaponRifle::StartSecondaryFire()
{
	Super::StartSecondaryFire();
	bSniperEnabled = !bSniperEnabled;

	if (bSniperEnabled)
	{
		Damage *= AltDamageMultiplier;
		TimeBetweenShots *= AltTimeBetweenShotsMultiplier;
	}
	else
	{
		Damage /= AltDamageMultiplier;
		TimeBetweenShots /= AltTimeBetweenShotsMultiplier;
	}

	AMachCharacter* Character = Cast<AMachCharacter>(GetOwner());
	if (Character)
	{
		Character->SetSniperCamera(bSniperEnabled);
	}
}