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

	UPROPERTY(Transient)
	class AMachWeapon* Weapon;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** setup velocity */
	void InitVelocity(FVector& ShootDirection);

	/** setup velocity */
	void SetVelocity(FVector& ShootDirection, float Velocity);

	UFUNCTION()
	virtual void OnCollisionOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	UPROPERTY(Transient)
	uint8 bExploded : 1;
	UPROPERTY(Transient)
	uint8 bInOverlap : 1;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement)
	class UProjectileMovementComponent* ProjectileMovement;

	void OnProjectileStop(const FHitResult& ImpactResult);

	/** Triggered when the projectile collides against an Actor. */
	UFUNCTION()
	virtual void ProjectileHitActor(class AActor* Actor, const FHitResult& Hit);

	/** Triggered when the projectile collides against a primitive component. */
	UFUNCTION()
	virtual void ProjectileHitComponent(class UPrimitiveComponent* Comp, const FHitResult& Hit);

	/** Triggered when the projectile can't move anymore (lost all velocity or hit a wall). */
	UFUNCTION()
	virtual void ProjectileStopped(const FHitResult& Hit);

	UFUNCTION()
	void Explode(const FHitResult& ImpactResult);

	TWeakObjectPtr<AController> MyController;
};

