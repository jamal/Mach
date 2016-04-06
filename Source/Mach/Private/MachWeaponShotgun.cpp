#include "Mach.h"
#include "MachWeaponShotgun.h"


AMachWeaponShotgun::AMachWeaponShotgun(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> LaserParticleObject(TEXT("ParticleSystem'/Game/Particles/P_FineLaser.P_FineLaser'"));
	if (LaserParticleObject.Object != NULL)
	{
		LaserParticle = (UParticleSystem*) LaserParticleObject.Object;
	}

	PrimaryActorTick.bCanEverTick = true;
	bFocusingFire = false;
	bLaserInit = false;

	MuzzleOffset = FVector(100, 0, 0);

	TimeBetweenShots = 1.0f;
	WeaponModelType = EWeaponModelType::Rifle;
	BurstMode = EWeaponBurstMode::Full;
	Damage = 5;
	Spread = 10.f;
	FocusMultiplier = 1.5f;
	FocusRate = .75f;
    FocusMin = .3f;

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

void AMachWeaponShotgun::BeginPlay()
{
	Super::BeginPlay();
}

void AMachWeaponShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Rot += 100 * DeltaTime;
	if (Rot > 360) {
		Rot = 0;
	}

	// Update FocusMultiplier on both client and server
	if (bFocusingFire)
	{
		if (FocusMultiplier > FocusMin)
		{
			FocusMultiplier -= FocusRate * DeltaTime;
		}
		if (FocusMultiplier < FocusMin)
		{
			FocusMultiplier = FocusMin;
		}

		// Display beam particles in client
		if (Role < ROLE_Authority)
		{
			if (ParticleComponent == nullptr || !ParticleComponent->bIsActive)
			{
				USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
				if (UseWeaponMesh)
				{
					ParticleComponent = UGameplayStatics::SpawnEmitterAttached(LaserParticle, UseWeaponMesh, MuzzleAttachPoint);
				}
			}

			FVector StartTrace;
			FRotator AimRot;
			GetViewPoint(StartTrace, AimRot);


			for (int i = 0; i < 5; ++i)
			{
				const FVector EndTrace = StartTrace + EndTraceForBullet(AimRot, BulletVectors[i*3+1]);
				const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
				if (Impact.bBlockingHit)
				{
					ParticleComponent->SetBeamTargetPoint(0, Impact.ImpactPoint, i);
				}
				else
				{
					ParticleComponent->SetBeamTargetPoint(0, EndTrace, i);
				}
			}
		}
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

	for (FVector BulletVector : BulletVectors)
	{
        const FVector EndTrace = StartTrace + EndTraceForBullet(AimRot, BulletVector);
		ProcessHit(WeaponTrace(StartTrace, EndTrace), StartTrace, EndTrace);
	}

    if (bFocusingFire)
    {
        bFocusingFire = false;
        UpdateLaserParticle();
    }
}

FVector AMachWeaponShotgun::EndTraceForBullet(FRotator AimRot, FVector BulletVector)
{
    FVector RotatedVector = BulletVector.RotateAngleAxis(Rot, FVector(1, 0, 0));

    if (bFocusingFire)
    {
		RotatedVector.Y *= FocusMultiplier;
		RotatedVector.Z *= FocusMultiplier;
    }

    return AimRot.RotateVector(RotatedVector) * Range;
}

void AMachWeaponShotgun::UpdateLaserParticle()
{
	if (Role < ROLE_Authority && !bFocusingFire)
	{
		FocusMultiplier = 1.5f;
		ParticleComponent->DestroyComponent();
	}
}

void AMachWeaponShotgun::StartSecondaryFire()
{
	Super::StartSecondaryFire();
	bFocusingFire = true;
}

void AMachWeaponShotgun::StopSecondaryFire()
{
	Super::StopSecondaryFire();
	bFocusingFire = false;
	UpdateLaserParticle();
}

void AMachWeaponShotgun::OnRep_FocusingFire()
{
	UpdateLaserParticle();
}

void AMachWeaponShotgun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMachWeaponShotgun, bFocusingFire, COND_SkipOwner);
}
