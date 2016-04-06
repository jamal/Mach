// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachWeapon.h"
#include "MachShieldWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachShieldWeapon : public AMachWeapon
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* ShieldMesh;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_ShieldEnabled)
	uint32 bShieldEnabled : 1;

	void OnBurstStarted();
	void OnBurstFinished();

	/** initial setup */
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnRep_ShieldEnabled();
	void StartShield();
	void StopShield();
	void HandleFiring();
};
