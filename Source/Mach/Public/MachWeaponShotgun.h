#pragma once

#include "MachWeapon.h"
#include "MachWeaponShotgun.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachWeaponShotgun : public AMachWeapon
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TArray<FVector> BulletVectors;

	/** number of pellets that are fired per shot. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	int32 NumPellets;
	
	/** Particle for laser */
	UPROPERTY(EditDefaultsOnly, Category = Shotgun)
	UParticleSystem* LaserParticle;

	void BeginPlay() override;

private:
	float Rot;
	float FocusMultiplier;
	float FocusRate;
    float FocusMin;
	bool bLaserInit;
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_FocusingFire)
	bool bFocusingFire;

	class UParticleSystemComponent* ParticleComponent;

	void Tick(float DeltaTime);
    FVector EndTraceForBullet(FRotator AimRot, FVector BulletVector);

	void UpdateLaserParticle();

	virtual void FireWeapon();
	void StartSecondaryFire() override;
	void StopSecondaryFire() override;
	
	UFUNCTION()
	void OnRep_FocusingFire();
	
};
