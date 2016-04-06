// Fill out your copyright notice in the Description page of Project Settings.

#include "Mach.h"
#include "MachWeapon.h"
#include "MachChargeProjectile.h"


AMachChargeProjectile::AMachChargeProjectile(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	ProjectileMovement->InitialSpeed = 10000.f;
	ProjectileMovement->MaxSpeed = 10000.f;
}


void AMachChargeProjectile::OnCollisionOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Charge gun overlap"));
	
	if (Weapon != NULL) {
		Weapon->ProcessHit(Damage, SweepResult, SweepResult.ImpactPoint, SweepResult.ImpactPoint);
	}

	Destroy();
}