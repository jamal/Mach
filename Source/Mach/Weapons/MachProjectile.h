// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "MachProjectile.generated.h"

UCLASS(config=Game)
class MACH_API AMachProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* ExplosionFX;

	UPROPERTY(Transient)
	float Damage;

	UPROPERTY(Transient)
	float DamageRadius;

	UPROPERTY(Transient)
	TSubclassOf<class UDamageType> DamageType;

	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** setup velocity */
	void InitVelocity(FVector& ShootDirection);

	/** setup velocity */
	void SetVelocity(FVector& ShootDirection, float Velocity);

	/** handle hit */
	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);

	UFUNCTION()
	void OnCollisionOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	UPROPERTY(Transient)
	uint8 bExploded : 1;
	UPROPERTY(Transient)
	uint8 bInOverlap : 1;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	TSubobjectPtr<USphereComponent> CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement)
	TSubobjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	void OnProjectileStop(const FHitResult& ImpactResult);

	UFUNCTION()
	void Explode(const FHitResult& ImpactResult);

	TWeakObjectPtr<AController> MyController;
};

