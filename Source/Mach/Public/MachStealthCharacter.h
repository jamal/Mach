// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachCharacter.h"
#include "MachStealthCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachStealthCharacter : public AMachCharacter
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsStealthed)
	uint32 bIsStealthed : 1;

protected:
	class UMaterialInterface* OriginalMaterial;
	class UMaterialInterface* OriginalMaterial1P;
	class UMaterialInterface* OriginalMaterialPW;
	class UMaterialInterface* OriginalMaterialSW;
	class UMaterialInstanceDynamic* CloakedMaterial;

	uint32 bFirstStealthDone : 1;

	bool bIsGrappling;
	bool bDidJustGrapple;

	float OriginalMaxWalkSpeed;
	float StealthCooldown;
	void Tick(float DeltaSeconds);
	void StartMovementSpecial();
	void UpdateStealth();

	UFUNCTION()
	void OnRep_IsStealthed();

	UFUNCTION()
	void StopStealth();

	void StartJump() override;
	void StartStealth() override;

	void OnReload();
	void OnStartFire();
	void OnStartUse();
	void ReceiveAnyDamage(float Damage, const class UDamageType * DamageType, class AController * EventInstigator, class AActor * DamageCauser);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopStealth();
};
