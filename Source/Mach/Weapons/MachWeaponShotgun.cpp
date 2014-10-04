#include "Mach.h"
#include "MachWeaponShotgun.h"


AMachWeaponShotgun::AMachWeaponShotgun(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	TimeBetweenShots = 1.0f;
	WeaponModelType = EWeaponModelType::Rifle;
	BurstMode = EWeaponBurstMode::Full;
	NumPellets = 10;
	Damage = 7;
	Spread = 20.f;
}

void AMachWeaponShotgun::FireWeapon()
{
	float Steps[] = { NumPellets*.2, NumPellets*.5, NumPellets };
	int CurStep = 0;
	
	FVector StartTrace;
	FRotator AimRot;
	GetViewPoint(StartTrace, AimRot);

	int Pellets = 0;
	for (int i = 0; i < NumPellets; ++i)
	{
		if (i >= Steps[CurStep])
		{
			CurStep++;
		}

		const float CurrentSpread = Spread * ((CurStep + 1) / 3.f);

		UE_LOG(LogTemp, Warning, TEXT("Spread at %.2f"), CurrentSpread);

		const int32 RandomSeed = FMath::Rand();
		FRandomStream WeaponRandomStream(RandomSeed);
		const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

		// TODO: This is too random, and there's a chance of all pellets still being near the middle
		// We need to pick between a min and max at each step rather than just increasing the max
		const FVector ShootDir = WeaponRandomStream.VRandCone(AimRot.Vector(), ConeHalfAngle, ConeHalfAngle);

		const FVector EndTrace = StartTrace + ShootDir * Range;

		const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

		ProcessHit(StartTrace, Impact);
	}
}
