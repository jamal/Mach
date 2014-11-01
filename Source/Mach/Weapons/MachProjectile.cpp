#include "Mach.h"


AMachProjectile::AMachProjectile(const class FPostConstructInitializeProperties& PCIP) 
	: Super(PCIP)
{
	// Use a sphere as a simple collision representation
	CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMachProjectile::OnCollisionOverlap);
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 10000.f;
	ProjectileMovement->MaxSpeed = 10000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bExploded = false;
	bReplicates = true;
	bReplicateInstigator = true;
	bReplicateMovement = true;
}

void AMachProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &AMachProjectile::OnImpact);
	CollisionComp->MoveIgnoreActors.Add(Instigator);

	SetLifeSpan(InitialLifeSpan);
	MyController = GetInstigatorController();
}

void AMachProjectile::InitVelocity(FVector& ShootDirection)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}

void AMachProjectile::OnImpact(const FHitResult& ImpactResult)
{
	if (!bExploded)
	{
		Explode(ImpactResult);
	}
}

void AMachProjectile::OnCollisionOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OVERLAP"));
	if (!bInOverlap)
	{
		bInOverlap = true;

		FHitResult Hit;
		if (bFromSweep)
		{
			Hit = SweepResult;
		}
		else
		{
			OtherComp->LineTraceComponent(Hit, GetActorLocation() - GetVelocity() * 10.0, GetActorLocation() + GetVelocity(), FCollisionQueryParams(GetClass()->GetFName(), CollisionComp->bTraceComplexOnMove, this));
		}

		Explode(Hit);
	}
}

void AMachProjectile::Explode(const FHitResult& ImpactResult)
{
	if (!bExploded) {
		const FVector NudgedImpactLocation = ImpactResult.ImpactPoint + ImpactResult.ImpactNormal * 10.0f;
		UGameplayStatics::ApplyRadialDamage(this, Damage, NudgedImpactLocation, DamageRadius, DamageType, TArray<AActor*>(), this, MyController.Get());

		bExploded = true;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorLocation());
		SetLifeSpan(1.0f);
	}
}
