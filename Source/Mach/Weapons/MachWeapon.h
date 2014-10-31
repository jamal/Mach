#pragma once

#include "GameFramework/Actor.h"
#include "MachWeapon.generated.h"

UENUM(WeaponState)
namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Charging,
		Equipping,
		Unequipped,
	};
}

/** Weapon burst mode. Full burst will continue refiring until the fire button is released. Semi burst mode will fire
for a short period of time and stop until the fire button is pressed again. Single burst mode will never refire,
similar to a pistol. Charge burst mode will charge up while the fire button is pressed and shoot when released. */
UENUM(WeaponBurstMode)
namespace EWeaponBurstMode
{
	enum Type
	{
		Full,
		Semi,
		Single,
		Charge
	};
}

/** The weapon model type determines the animation that is needed to hold it (pistol, rifle) */
UENUM(BlueprintType)
namespace EWeaponModelType
{
	enum Type
	{
		Rifle,
		Pistol,
	};
}

/**
 * Base weapon class.
 */
UCLASS()
class AMachWeapon : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	TSubobjectPtr<class USkeletalMeshComponent> Mesh3P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	TSubobjectPtr<class USkeletalMeshComponent> Mesh1P;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	TSubclassOf<class AMachProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	FVector MuzzleOffset;

	/** Damage per shot / bullet */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float Damage;

	/** Max ammo per clip */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	int32 MaxAmmo;

	/** Total number of clips that can be carried at a time */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	int32 AmmoPerClip;

	/** Time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float TimeBetweenShots;

	/** Time between two bursts in semi burst mode */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float TimeBetweenSemiBursts;

	/** Weapon burst mode */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	TEnumAsByte<EWeaponBurstMode::Type> BurstMode;

	/** Range of the weapon (Default: 12000) */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float Range;

	/** Weapon spread, if greater than 0 will apply a random spread */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float Spread;

	/** This is awkard and weird, we should have animations for each weapon specifically */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = WeaponStat)
	TEnumAsByte<EWeaponModelType::Type> WeaponModelType;

	/** Impact effects */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<class AMachImpactEffect> ImpactTemplate;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	void SetOwningPawn(class AMachCharacter* NewPawn);
	void OnEquip();
	void OnUnequip();

	UFUNCTION()
	void StartFire();

	UFUNCTION()
	void StopFire();
	
	UFUNCTION()
	void Reload();

protected:
	UPROPERTY(Transient, Replicated)
	TEnumAsByte<EWeaponState::Type> CurrentState;

	UPROPERTY(Transient, Replicated)
	AMachCharacter* OwnerPawn;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category=Gameplay)
	int32 TotalAmmo;
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category=Gameplay)
	int32 Ammo;

	uint8 bIsEquipped : 1;

	uint8 bEquipPending : 1;
	uint8 bFireIntent : 1;
	uint8 bReloadIntent : 1;
	uint8 bRefiring : 1;

	float fLastFireTime;
	float fLastBurstTime;

	UPROPERTY(Transient, Replicated)
	FHitResult HitImpact;

	virtual void PostInitializeComponents() override;

	bool CanReload();
	bool CanFire();

	void OnEquipFinished();
	void UpdateWeaponState();
	void SetCurrentState(EWeaponState::Type);
	void OnBurstStarted();
	void OnBurstFinished();
	void OnReloadStarted();
	void OnReloadFinished();
	void HandleFiring();
	FVector GetViewRotation();
	void GetViewPoint(FVector& start, FRotator& rotation);
	void SpawnImpactEffects(const FHitResult& impact);
	virtual void FireWeapon();
	void ProcessHit(FVector Start, FHitResult Impact);
	FHitResult WeaponTrace(const FVector& start, const FVector& end) const;
	void AttachMesh();
	void DetachMesh();
	void ConsumeAmmo();
	void FireProjectile();

	void NotifyHit(const FHitResult& impact, const FVector& origin);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();

	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult impact, const FVector origin);

	/** Burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_BurstCounter)
	int32 BurstCounter;

	//////////////////////////////////////////////////////////////////////////
	// Replication
	
	UFUNCTION()
	void OnRep_BurstCounter();

};