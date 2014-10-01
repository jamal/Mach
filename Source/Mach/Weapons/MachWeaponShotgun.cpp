

#include "Mach.h"
#include "MachWeaponShotgun.h"


AMachWeaponShotgun::AMachWeaponShotgun(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	TimeBetweenShots = 1.0f;
	WeaponModelType = EWeaponModelType::Rifle;
	BurstMode = EWeaponBurstMode::Full;
}
