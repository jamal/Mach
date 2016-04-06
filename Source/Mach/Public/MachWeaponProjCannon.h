// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachWeapon.h"
#include "MachWeaponProjCannon.generated.h"

UENUM(ProjCannonBuildable, BlueprintType)
namespace EProjCannonBuildable
{
	enum Type
	{
		None,
		Turret,
		Vendbot,
	};
}


/**
 * 
 */
UCLASS()
class MACH_API AMachWeaponProjCannon : public AMachWeapon
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = ProjCannon)
	float Healing;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Active)
	bool bActive;

	UPROPERTY(EditDefaultsOnly, Category = ProjCannon)
	class UParticleSystem* BeamParticle;

	/** The widget is created and assigned in the weapon blueprint. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjCannon)
	class UMachProjCannonPickerWidget* PickerWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = ProjCannon)
	TSubclassOf<class AMachTurret> Turret;

	UPROPERTY(EditDefaultsOnly, Category = ProjCannon)
	TSubclassOf<class AMachVendbot> Vendbot;

	void PostInitializeComponents() override;

	void FireWeapon() override;

	/** Determine if an actor that is overlapping the fire range can actually be hit. */
	void CanActorBeHit();

	void StartFire() override;
	void StopFire() override;

	void StartSecondaryFire() override;
	void StopSecondaryFire() override;

	void UpdateParticleComponent();
	void Tick(float DeltaTime);

	UFUNCTION()
	void OnRep_Active();

	void OnBuildableDestroyed(class AMachBuildable* Buildable);

protected:
	class AMachBuildable* CurrentBuild;
	class UParticleSystemComponent* ParticleComponent;
	
	UPROPERTY()
	int8 MaxTurrets;

	UPROPERTY(Transient, Replicated)
	int8 NumTurrets;
	
	UPROPERTY()
	int8 MaxVendbots;

	UPROPERTY(Transient, Replicated)
	int8 NumVendbots;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDeploy(EProjCannonBuildable::Type Buildable, FVector Location, FRotator Rotation);
};
