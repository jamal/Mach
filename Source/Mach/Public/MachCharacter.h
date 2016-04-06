// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "MachGameState.h"
#include "MachTechPointListenerInterface.h"
#include "UMG.h"
#include "MachCharacter.generated.h"

UCLASS(config=Game)
class AMachCharacter : public ACharacter, public IMachTechPointListenerInterface
{
	GENERATED_UCLASS_BODY()

	/** Setup inventory, and whatever else when a character spawns. */
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	void Destroyed() override;

	FVector ServerJumpMovementInput;

	/** Damage bonus from implants and such. */
	UPROPERTY(Transient, Replicated)
	float DamageBonusMultiplier;

	/** Number of kills since last death. */
	UPROPERTY(Transient, Replicated)
	int32 Killstreak;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	class UCameraComponent* Camera1P;

	/** Not completely implemented. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=HUD)
	class UParticleSystemComponent* HUDParticles;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	class USpringArmComponent* ThirdPersonCameraArm;

	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	class UCameraComponent* Camera3P;

	/** Sniper camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	class UCameraComponent* CameraSniper;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=MachCharacter)
	class USphereComponent* OverHeadComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UMaterialInstance* MaterialTeamA;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UMaterialInstance* MaterialTeamB;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	float BaseHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	float BaseShield;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	float BaseEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	float MovementSpecialCost;

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
	
	UPROPERTY(Transient, Replicated)
	bool bIsVisibleToEnemy;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_IsBeingTracked)
	bool bIsBeingTracked;

	/** Default weapon is only really used during development -- inventory should be configured differently */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TSubclassOf<class AMachWeapon> DefaultPrimaryWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TSubclassOf<class AMachWeapon> DefaultSecondaryWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TSubclassOf<class AMachWeapon> DefaultTertiaryWeapon;

	TSubclassOf<class AMachWeapon> Rifle;
	TSubclassOf<class AMachWeapon> Shotgun;
	TSubclassOf<class AMachWeapon> EnergyGun;
	TSubclassOf<class AMachWeapon> GrenadeLauncher;
	TSubclassOf<class AMachWeapon> ProjCannon;

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

	UPROPERTY(Transient, Replicated)
	class AMachHelmet* Helmet;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category=Gameplay)
	float Health;
	
	/** Current maximum shield based on power level. */
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category=Gameplay)
	float MaxShield;
	
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category=Gameplay)
	float Shield;
	
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category=Gameplay)
	float Energy;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category=Gameplay)
	float EnergyRegenRate;

	/** How much time has passed since the last time we took damage. */
	UPROPERTY(BlueprintReadOnly, Category=Gameplay)
	float TimeSinceLastDamageTaken;

	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	class UMachImplantComponent* Implant;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category=Animation)
	float Pitch;

	FName GetWeaponAttachPoint() const;

	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = Gameplay)
	void KillPlayer();

	bool IsUsePressed();
	bool IsFirstPerson() const;

	void UpdatePrimaryWeapon();
	
	void StartSprinting();
	void StopSprinting();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	ETeam::Type GetTeam();

	virtual void StartJump();
	virtual void StopJump();

	virtual void StartStealth();

	/** Triggered when jump input is double pressed quickly. */
	virtual void OnDoubleJump();

	/** Triggered when the Reload button is pressed */
	virtual void OnReload();

	/** Triggered when the Fire button is pressed */
	virtual void OnStartFire();
	/** Triggered when the Fire button is released */
	virtual void OnStopFire();

	virtual void OnStartSecondaryFire();
	virtual void OnStopSecondaryFire();

	/** Handles moving forward/backward */
	virtual void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	virtual void MoveRight(float Val);

	void Equip1();
	void Equip2();
	void Equip3();
	void EquipLast();

	virtual void OnStartUse();
	virtual void OnStopUse();
	virtual void OnCrouchStart();
	virtual void OnCrouchStop();

	virtual void StartMovementSpecial();
	virtual void StopMovementSpecial();

	virtual void UseVision();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SpawnNewPrimaryWeapon(TSubclassOf<class AMachWeapon> WeaponClass);
	
	UFUNCTION(BlueprintCallable, Category=MachCharacter)
	float GetHealthPercent();
	UFUNCTION(BlueprintCallable, Category=MachCharacter)
	float GetShieldPercent();
	UFUNCTION(BlueprintCallable, Category=MachCharacter)
	float GetEnergyPercent();
	
	/** Set the player's shield multiplier and start charging if needed. */
	void SetShieldMultiplier(float Multiplier);

	void SetSniperCamera(bool bEnabled);

	/** Called by the tracking helmet when this character is hit by a tracker. */
	UFUNCTION()
	void StartTracker();

	/** Called by a timer to stop the tracker. */
	UFUNCTION()
	void StopTracker();

protected:

	class UMachCharacterMovement* MachCharacterMovement;

	/** Character is trying to "use" an object (use key is pressed) */
	UPROPERTY(Transient, Replicated)
	uint8 bIsUsePressed : 1;

	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category=Animation)
	uint8 bIsJumpPressed : 1;

	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category=Animation)
	uint8 bIsCrouching : 1;
	
	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category = Animation)
	uint8 bIsDying : 1;

	UPROPERTY(BlueprintReadOnly, Transient, Replicated, Category = Animation)
	uint8 bCanMove : 1;

	UPROPERTY(BlueprintReadOnly, replicatedUsing=OnRep_IsSprinting, Category=Character)
	uint8 bIsSprinting : 1;

	float SniperTurnRateModifier;

	bool bIsSniperCameraEnabled;

	float LastJumpTime;

	UClass* NameplateClass;

	UPROPERTY(BlueprintReadWrite, Category=Character)
	class UMachNameplateWidget* NameplateWidget;

	UClass* DamageTextWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = Character)
	class UMachDamageTextWidget* DamageTextWidget;

	UFUNCTION()
	void OnRep_IsSprinting();

	uint8 bFireIntent : 1;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SetCurrentWeapon(class AMachWeapon* Weapon);

	void OnDeath();

	bool ShouldTakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) const;
	void ReceiveAnyDamage(float Damage, const class UDamageType * DamageType, class AController * EventInstigator, class AActor * DamageCauser);
	void RegenerateShield();

	void ClearDamageTakenText();

	virtual void Tick(float DeltaSeconds);
	
	virtual void SetPlayerDefaults();

	void UpdateCameras();
	void UpdateMeshes();
	void SetThirdPerson(bool bIsThirdPerson);

	UPROPERTY(Transient, Replicated)
	bool bSpendingEnergy;
	uint32 bCamera3P : 1;

	virtual bool CanDoMovementSpecial();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_DamageTaken)
	uint32 DamageTaken;

	UPROPERTY(Transient, Replicated)
	AActor* DamageTakenCauser;

	UFUNCTION()
	void OnRep_IsBeingTracked();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartMovementSpecial();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopMovementSpecial();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetCurrentWeapon(class AMachWeapon* NewWeapon);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnNewPrimaryWeapon(TSubclassOf<class AMachWeapon> WeaponClass);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnStartUse();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnStopUse();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnStartJump(FVector InputVector);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnStopJump();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartStealth();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartSprinting();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopSprinting();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDoubleJump();
	
	void UpdateShieldMultiplier();
	void OnTechPointCaptured();
	void OnTechPointLost();

	//////////////////////////////////////////////////////////////////////////
	// Replication

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_CurrentWeapon(class AMachWeapon* LastWeapon);

	UFUNCTION()
	void OnRep_DamageTaken();

	TSubclassOf<class AMachWeapon> GetPrimaryWeaponClass();
};

