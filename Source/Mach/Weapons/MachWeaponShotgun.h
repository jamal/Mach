#pragma once

#include "Weapons/MachWeapon.h"
#include "MachWeaponShotgun.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachWeaponShotgun : public AMachWeapon
{
	GENERATED_UCLASS_BODY()

	/** number of pellets that are fired per shot. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	int32 NumPellets;

private:
	virtual void FireWeapon();
	
};
