// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "MachGameState.h"
#include "MachWeaponProjCannon.h"
#include "MachBuildable.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachBuildable : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, Category=Buildable)
	TEnumAsByte<EProjCannonBuildable::Type> BuildableType;
	
	float Health;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	TEnumAsByte<ETeam::Type> Team;

	UPROPERTY(EditDefaultsOnly, Category=Buildable)
	class UStaticMeshComponent* Mesh;

	/** How far the buildable can be deployed. */
	UPROPERTY(EditAnywhere, Category=Buildable)
	float Range;
	
	UPROPERTY(EditAnywhere, Category=Buildable)
	class UMaterialInterface* GoodMaterial;

	UPROPERTY(EditAnywhere, Category=Buildable)
	class UMaterialInterface* BadMaterial;

	/** Update the position of the buildable item while previewing. */
	bool SetPreviewLocation(const FHitResult& Hit);

	bool CanBeDeployed();
	bool CanBeDeployed(FVector Location, FRotator Rotation);

	/** Deploy the buildable. */
	virtual bool Deploy();

	void Destroyed() override;

protected:

	bool bIsActive;
	bool bIsDeployed;
	class UMaterialInterface* OriginalMaterial;

	virtual void ReceiveAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser) override;
	
};
