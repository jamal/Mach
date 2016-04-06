// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "MachBuildable.h"
#include "MachTurret.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachTurret : public AMachBuildable
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Turret)
	float FireRange;

	UPROPERTY(EditDefaultsOnly, Category = Turret)
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category = Turret)
	float Damage;
	
	UPROPERTY(VisibleAnywhere, Category = Turret)
	class USphereComponent* SphereComponent;

protected:
	TArray<class AActor*> ActorsInRange;

	UPROPERTY(Transient, Replicated)
	class AActor* CurrentTarget;

	UFUNCTION()
 	void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
 	void OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Fire();
};
