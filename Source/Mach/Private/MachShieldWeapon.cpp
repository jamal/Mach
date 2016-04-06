// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachShieldWeapon.h"


AMachShieldWeapon::AMachShieldWeapon(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	MaxAmmo = 100;
	AmmoPerClip = 100;
	TimeBetweenShots = 0.1f;
	TimeBetweenSemiBursts = 0.1f;
	BurstMode = EWeaponBurstMode::Full;
	Range = 12000.f;
	BurstCounter = 0;
	Resource = EWeaponResource::Energy;
	EnergyRate = 6.67f;
	EnergyRechargeRate = 4.f;

	bShieldEnabled = true;

	bAlwaysRelevant = true;

	RootComponent = Arrow;

	ShieldMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ShieldMesh"));
	StopShield();
}

void AMachShieldWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ShieldMesh->MoveIgnoreActors.Add(Instigator);
	ShieldMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}

void AMachShieldWeapon::StartShield()
{
	ShieldMesh->AttachTo(GetWeaponMesh(), MuzzleAttachPoint);
	UE_LOG(LogTemp, Warning, TEXT("Shield started"));
	ShieldMesh->SetHiddenInGame(false);
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	bShieldEnabled = true;
}

void AMachShieldWeapon::StopShield()
{
	UE_LOG(LogTemp, Warning, TEXT("Shield stopped"));
	ShieldMesh->SetHiddenInGame(true);
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bShieldEnabled = false;
}

void AMachShieldWeapon::OnBurstStarted()
{
	fLastBurstTime = GetWorld()->GetTimeSeconds();
	StartShield();
}

void AMachShieldWeapon::OnBurstFinished()
{
	StopShield();
}

void AMachShieldWeapon::HandleFiring()
{
	// Don't do anything
}

void AMachShieldWeapon::OnRep_ShieldEnabled()
{
	bShieldEnabled ? StartShield() : StopShield();
}

void AMachShieldWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachShieldWeapon, bShieldEnabled);
}
