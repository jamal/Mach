#pragma once

#include "GameFramework/Actor.h"
#include "MachImpactEffect.generated.h"

// TODO: This needs to be somewhere else
USTRUCT()
struct FDecalData
{
	GENERATED_USTRUCT_BODY()

	/** material */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
	UMaterial* DecalMaterial;

	/** quad size (width & height) */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
	float DecalSize;

	/** lifespan */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
	float LifeSpan;

	/** defaults */
	FDecalData()
		: DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}
};

/**
 * 
 */
UCLASS()
class AMachImpactEffect : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UParticleSystem* DefaultFX;

	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UParticleSystem* ShieldFX;

	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UParticleSystem* FleshFX;

	/** default decal when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	struct FDecalData DefaultDecal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category = Surface)
	FHitResult SurfaceHit;

	/** spawn effect */
	virtual void PostInitializeComponents() override;
	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;
};
