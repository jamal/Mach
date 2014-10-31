#include "Mach.h"
#include "MachWeaponShotgun.h"


AMachWeaponShotgun::AMachWeaponShotgun(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;

	MuzzleOffset = FVector(100, 0, 0);

	TimeBetweenShots = 1.0f;
	WeaponModelType = EWeaponModelType::Rifle;
	BurstMode = EWeaponBurstMode::Full;
	Damage = 5;
	Spread = 10.f;

	BulletVectors.Add(FVector(1, 0.001, -0.001));

	BulletVectors.Add(FVector(1, 0.035, 0.035));
	BulletVectors.Add(FVector(1, -0.035, 0.035));
	BulletVectors.Add(FVector(1, -0.035, -0.035));
	BulletVectors.Add(FVector(1, 0.035, -0.035));

	BulletVectors.Add(FVector(1, 0.045, 0));
	BulletVectors.Add(FVector(1, 0, 0.045));
	BulletVectors.Add(FVector(1, -0.045, 0));
	BulletVectors.Add(FVector(1, 0, -0.045));

	BulletVectors.Add(FVector(1, 0.07, 0.07));
	BulletVectors.Add(FVector(1, -0.07, 0.07));
	BulletVectors.Add(FVector(1, -0.07, -0.07));
	BulletVectors.Add(FVector(1, 0.07, -0.07));

	BulletVectors.Add(FVector(1, 0.09, 0));
	BulletVectors.Add(FVector(1, 0, 0.09));
	BulletVectors.Add(FVector(1, -0.09, 0));
	BulletVectors.Add(FVector(1, 0, -0.09));
}

void AMachWeaponShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Rot += 100 * DeltaTime;
	if (Rot > 360) {
		Rot = 0;
	}
}

void AMachWeaponShotgun::FireWeapon()
{
	if (!CanFire())
	{
		return;
	}

	ConsumeAmmo();

	FVector StartTrace;
	FRotator AimRot;
	GetViewPoint(StartTrace, AimRot);

	FRotationMatrix AimRotMatrix(AimRot);

	for (FVector BulletVector : BulletVectors)
	{
		FVector RotatedVector = BulletVector.RotateAngleAxis(Rot, FVector(1, 0, 0));
		const FVector EndTrace = AimRotMatrix.TransformVector(StartTrace + RotatedVector * Range);

		const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
		ProcessHit(Impact, StartTrace, EndTrace);
	}
}
