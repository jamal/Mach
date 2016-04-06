// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachStealthCharacter.h"
#include "MachWeapon.h"
#include "MachStealthCharacterMovement.h"
#include "MachNameplateWidget.h"

AMachStealthCharacter::AMachStealthCharacter(const class FObjectInitializer& PCIP)
	: Super(PCIP.SetDefaultSubobjectClass<UMachStealthCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	BaseHealth = 150.0f;
	BaseShield = 75.0f;
	ShieldRegenStartTime = 5.0f;
	ShieldRegenRate = 20.0f;
	MovementSpecialCost = 0;

	bSpendingEnergy = true;
	bIsStealthed = false;
	bFirstStealthDone = false;
	bIsGrappling = false;
	bDidJustGrapple = false;

	StealthCooldown = 0.f;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CloakedMaterialObject(TEXT("Material'/Game/Materials/CloakingTest.CloakingTest'"));
	if (CloakedMaterialObject.Object != NULL)
	{
		UMaterialInterface* CloakedMaterialInterface = (UMaterialInterface*)CloakedMaterialObject.Object;
		CloakedMaterial = UMaterialInstanceDynamic::Create(CloakedMaterialInterface, this);
	}
}

void AMachStealthCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDidJustGrapple)
	{
		bDidJustGrapple = false;
	}

	if (StealthCooldown > 0.f)
	{
		StealthCooldown -= DeltaSeconds;
	}

	if (bIsStealthed)
	{
		if (CharacterMovement)
		{
			float VelRatio = CharacterMovement->Velocity.Size() / CharacterMovement->MaxWalkSpeed;
			CloakedMaterial->SetScalarParameterValue("Refraction", VelRatio * .2f + 1.02f);
		}

		Energy -= 3.33f * DeltaSeconds;
		if (Energy <= 0)
		{
			StopStealth();
		}
	}
}

void AMachStealthCharacter::StartStealth()
{
	Super::StartStealth();

	if (StealthCooldown <= 0.f)
	{
		StealthCooldown = 1.f;
		bIsStealthed = !bIsStealthed;
		bSpendingEnergy = bIsStealthed;
		UpdateStealth();
	}
}

void AMachStealthCharacter::UpdateStealth()
{
	if (bIsStealthed)
	{
		if (GetNetMode() == NM_Client && NameplateWidget)
		{
			NameplateWidget->RemoveFromViewport();
		}

		if (!bFirstStealthDone)
		{
			bFirstStealthDone = true;

			OriginalMaterial = Mesh->GetMaterial(0);
			OriginalMaterial1P = Mesh1P->GetMaterial(0);
			OriginalMaterialPW = PrimaryWeapon->GetWeaponMesh()->GetMaterial(0);
			OriginalMaterialSW = PrimaryWeapon->GetWeaponMesh()->GetMaterial(0);
		}

		Mesh->SetMaterial(0, CloakedMaterial);
		Mesh1P->SetMaterial(0, CloakedMaterial);
		PrimaryWeapon->GetWeaponMesh()->SetMaterial(0, CloakedMaterial);
		SecondaryWeapon->GetWeaponMesh()->SetMaterial(0, CloakedMaterial);
	}
	else
	{
		if (GetNetMode() == NM_Client && NameplateWidget)
		{
			NameplateWidget->AddToViewport();
		}

		Mesh->SetMaterial(0, OriginalMaterial);
		Mesh1P->SetMaterial(0, OriginalMaterial1P);
		PrimaryWeapon->GetWeaponMesh()->SetMaterial(0, OriginalMaterialPW);
		SecondaryWeapon->GetWeaponMesh()->SetMaterial(0, OriginalMaterialSW);
	}
}

void AMachStealthCharacter::OnRep_IsStealthed()
{
	UpdateStealth();
}

// Events that break stealth
void AMachStealthCharacter::OnReload()
{
	StopStealth();
	Super::OnReload();
}

void AMachStealthCharacter::OnStartFire()
{
	StopStealth();
	Super::OnStartFire();
}

void AMachStealthCharacter::OnStartUse()
{
	StopStealth();
	Super::OnStartUse();
}

void AMachStealthCharacter::ReceiveAnyDamage(float Damage, const class UDamageType * DamageType, class AController * EventInstigator, class AActor * DamageCauser)
{
	StopStealth();
	Super::ReceiveAnyDamage(Damage, DamageType, EventInstigator, DamageCauser);
}

void AMachStealthCharacter::StopStealth()
{
	if (Role < ROLE_Authority)
	{
		ServerStopStealth();
	}

	bIsStealthed = false;
	bSpendingEnergy = false;
	UpdateStealth();
}

bool AMachStealthCharacter::ServerStopStealth_Validate()
{
	return true;
}

void AMachStealthCharacter::ServerStopStealth_Implementation()
{
	StopStealth();
}

void AMachStealthCharacter::StartJump()
{
	if (MachCharacterMovement->MovementMode == MOVE_Custom && bIsGrappling && !bDidJustGrapple)
	{
		UMachStealthCharacterMovement* StealthMovement = Cast<UMachStealthCharacterMovement>(MachCharacterMovement);
		if (StealthMovement)
		{
			bIsGrappling = false;
			StealthMovement->StopGrapple();
		}
	}

	Super::StartJump();
}

void AMachStealthCharacter::StartMovementSpecial()
{
	Super::StartMovementSpecial();

	if (bIsStealthed)
	{
		StopStealth();
	}

	FVector Start, End;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Start, Rotation);
	End = Start + 10000 * Rotation.Vector();

	const FName TraceTag("StealthGrapple");
	FCollisionQueryParams TraceParams(TraceTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	
	FHitResult Impact(ForceInit);
	GetWorld()->LineTraceSingle(Impact, Start, End, ECC_Pawn, TraceParams);
	if (Impact.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grapple hit a wall"));
		UMachStealthCharacterMovement* StealthMovement = Cast<UMachStealthCharacterMovement>(MachCharacterMovement);
		if (StealthMovement)
		{
			bIsGrappling = true;
			bDidJustGrapple = true;
			StealthMovement->StartGrapple(Impact);
		}
	}
}

void AMachStealthCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachStealthCharacter, bIsStealthed);
}
