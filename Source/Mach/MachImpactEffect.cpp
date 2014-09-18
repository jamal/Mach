#include "Mach.h"
#include "MachImpactEffect.h"
#include "MachTypes.h"


AMachImpactEffect::AMachImpactEffect(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

void AMachImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}

	if (DefaultDecal.DecalMaterial)
	{
		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(DefaultDecal.DecalSize, DefaultDecal.DecalSize, 1.0f),
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
			DefaultDecal.LifeSpan);
	}
}

UParticleSystem* AMachImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	switch (SurfaceType)
	{
	case MACH_SURFACE_Shield:		return ShieldFX;
	case MACH_SURFACE_Flesh:		return FleshFX;
	default:						return DefaultFX;
	}
}