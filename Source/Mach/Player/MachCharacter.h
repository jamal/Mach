// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "MachCharacter.generated.h"

UCLASS(config=Game)
class AMachCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** Setup inventory, and whatever else when a character spawns. */
	virtual void PostInitializeComponents() override;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	TSubobjectPtr<class USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	TSubobjectPtr<class UCameraComponent> FirstPersonCameraComponent;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	float BaseHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	float BaseShield;

	/** Time it takes for the shield to start regenerating after damage was taken in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	float ShieldRegenStartTime;

	/** Amount of shield to regen per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	float ShieldRegenRate;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AMachProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Default weapon is only really used during development -- inventory should be configured differently */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<class AMachWeapon> DefaultPrimaryWeapon;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<class AMachWeapon> DefaultSecondaryWeapon;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<class AMachWeapon> DefaultTertiaryWeapon;

	/** Currently equipped weapon */
	UPROPERTY(BlueprintReadOnly, Category=Inventory, Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class AMachWeapon* CurrentWeapon;

	class AMachWeapon* LocalLastWeapon;

	UPROPERTY(BlueprintReadWrite, Transient, Replicated, Category = Weapons)
	class AMachWeapon* PrimaryWeapon;

	UPROPERTY(BlueprintReadWrite, Transient, Replicated, Category = Weapons)
	class AMachWeapon* SecondaryWeapon;

	UPROPERTY(Transient, Replicated)
	class AMachWeapon* TertiaryWeapon;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category=Gameplay)
	float Health;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category=Gameplay)
	float Shield;

	FName GetWeaponAttachPoint() const;

	bool IsUsePressed();

	ETeam::Type GetTeam();

protected:

	/** Character is trying to "use" an object (use key is pressed) */
	UPROPERTY(Transient, Replicated)
	uint8 bIsUsePressed : 1;

	uint8 bFireIntent : 1;
	uint8 bIsDying : 1;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;

	/** Triggered when the Reload button is pressed */
	void OnReload();

	/** Triggered when the Fire button is pressed */
	void OnStartFire();
	/** Triggered when the Fire button is released */
	void OnStopFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	void SetCurrentWeapon(class AMachWeapon* Weapon);

	void Equip1();
	void Equip2();
	void Equip3();
	void EquipLast();

	void OnStartUse();
	void OnStopUse();

	void OnDeath();

	bool ShouldTakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) const;
	void ReceiveAnyDamage(float Damage, const class UDamageType * DamageType, class AController * EventInstigator, class AActor * DamageCauser);
	void RegenerateShield();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetCurrentWeapon(class AMachWeapon* NewWeapon);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnStartUse();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnStopUse();

	//////////////////////////////////////////////////////////////////////////
	// Replication

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_CurrentWeapon(class AMachWeapon* LastWeapon);
};

