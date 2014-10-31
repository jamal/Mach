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

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TArray<FVector> BulletVectors;

	/** number of pellets that are fired per shot. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	int32 NumPellets;

private:
	float Rot;

	void Tick(float DeltaTime);

	virtual void FireWeapon();
	
};
