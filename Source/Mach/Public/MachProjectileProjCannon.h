// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachProjectile.h"
#include "MachProjectileProjCannon.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachProjectileProjCannon : public AMachProjectile
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category=ProjCannon)
	class UParticleSystemComponent* ProjectileParticle;

	UPROPERTY(EditDefaultsOnly, Category = ProjCannon)
	class UParticleSystemComponent* HitParticleFriendly;
	
	UPROPERTY(EditDefaultsOnly, Category=ProjCannon)
	class UParticleSystemComponent* HitParticleEnemy;
	
protected:
	void ProjectileHitActor(class AActor* Actor, const FHitResult& Hit) override;
	void ProjectileHitComponent(class UPrimitiveComponent* Comp, const FHitResult& Hit) override;
	void ProjectileStopped(const FHitResult& Hit) override;
	
};
