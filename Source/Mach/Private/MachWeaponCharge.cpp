// Fill out your copyright notice in the Description page of Project Settings.

#include "Mach.h"
#include "MachProjectile.h"
#include "MachWeaponCharge.h"

AMachWeaponCharge::AMachWeaponCharge(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	BurstMode = EWeaponBurstMode::Charge;
}


void AMachWeaponCharge::HandleFiring()
{
	Super::HandleFiring();
	// TODO: Start the "charge" animation

	/*
	if (Role == ROLE_Authority) {
		FVector AimLoc;
		FRotator AimRot;
		GetViewPoint(AimLoc, AimRot);

		FVector Muzzle = GetMuzzleLocation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Instigator;
		SpawnParams.Instigator = Instigator;

		ChargeProjectile = Cast<AMachProjectile>(GetWorld()->SpawnActor<AMachProjectile>(ProjectileClass, Muzzle, AimRot, SpawnParams));
		if (ChargeProjectile)
		{
			//ChargeProjectile->CollisionComp->SetSimulatePhysics(false);
			//ChargeProjectile->CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ChargeProjectile->AttachRootComponentTo(GetWeaponMesh(), MuzzleAttachPoint, EAttachLocation::SnapToTarget);
			ChargeProjectile->Damage = Damage;
			ChargeProjectile->DamageRadius = DamageRadius;
			ChargeProjectile->DamageType = UDamageType::StaticClass();
		}
	}
	*/
}

void AMachWeaponCharge::FireWeapon()
{
	Super::FireWeapon();
}

void AMachWeaponCharge::FireProjectile()
{
	const float Charged = GetWorld()->GetTimeSeconds() - fLastBurstTime;
	const float DealingDamage = FMath::Max<float>(Damage, MaxDamage * FMath::Min<float>((Charged / ChargeTime), 1.f));
	UE_LOG(LogTemp, Warning, TEXT("Fired for %.2f for %.2f damage"), Charged, DealingDamage);

	FVector AimLoc;
	FRotator AimRot;
	GetViewPoint(AimLoc, AimRot);

	FVector Muzzle = GetMuzzleLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Instigator;
	SpawnParams.Instigator = Instigator;

	// TODO: This is probably going to work like shit using a remote server
	if (Role == ROLE_Authority) {
		AMachProjectile* Projectile = Cast<AMachProjectile>(GetWorld()->SpawnActor<AMachProjectile>(ProjectileClass, Muzzle, AimRot, SpawnParams));
		if (Projectile)
		{
			Projectile->Weapon = this;
			Projectile->Damage = DealingDamage;
			Projectile->DamageRadius = DamageRadius;
			Projectile->DamageType = UDamageType::StaticClass();
		}
		// Replicate weapon firing
		BurstCounter++;
	}

	SimulateWeaponFiring();
}