// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "MachProjectile.generated.h"

UCLASS(config=Game)
class AMachProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* ExplosionFX;

	/** time from spawn before the projectile explodes (grenade style), set to 0 to disable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float ExplodeTime;

	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** setup velocity */
	void InitVelocity(FVector& ShootDirection);

	/** handle hit */
	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);

protected:
	UPROPERTY(Transient)
	uint8 bExploded : 1;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	TSubobjectPtr<USphereComponent> CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement)
	TSubobjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	void OnProjectileStop(const FHitResult& ImpactResult);
	void SelfExplode();


	TWeakObjectPtr<AController> MyController;
};

