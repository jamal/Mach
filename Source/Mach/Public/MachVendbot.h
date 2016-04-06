// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachBuildable.h"
#include "MachVendbot.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachVendbot : public AMachBuildable
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Vendbot)
	float VendbotRange;

	UPROPERTY(EditDefaultsOnly, Category = Vendbot)
	float Rate;

	UPROPERTY(EditDefaultsOnly, Category = Vendbot)
	float HealthPerTick;
	
	UPROPERTY(VisibleAnywhere, Category = Vendbot)
	class USphereComponent* SphereComponent;

protected:
	TArray<class AActor*> ActorsInRange;

	UFUNCTION()
 	void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
 	void OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Regen();
};
