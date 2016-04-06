// Fill out your copyright notice in the Description page of Project Settings.

#include "Mach.h"
#include "MachHeavyCharacter.h"
#include "MachHeavyCharacterMovement.h"
#include "MachPlayerController.h"

AMachHeavyCharacter::AMachHeavyCharacter(const class FObjectInitializer& PCIP)
: Super(PCIP.SetDefaultSubobjectClass<UMachHeavyCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	ChargeTrigger = PCIP.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("ChargeTrigger"));
	ChargeTrigger->InitCapsuleSize(96.f, 96.f);
	ChargeTrigger->AttachParent = RootComponent;
	ChargeTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMachHeavyCharacter::OnChargeTriggerOverlap);

	BaseHealth = 200.0f;
	BaseShield = 100.0f;
	ShieldRegenStartTime = 5.0f;
	ShieldRegenRate = 20.0f;
	LastJumpTime = 0.f;

	MovementSpecialCost = 10.f;

	LeapHitHorizontalImpulse = 400.f;
	LeapHitVerticalImuplse = 100.f;

	ChargeHitHorizontalImpulse = 200.f;
	ChargeHitVerticalImuplse = 75.f;

	LeapCost = 75.f;

	ChargeTurnRateModifier = 10.f;
}

void AMachHeavyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bSpendingEnergy)
	{
		if (MachCharacterMovement->CustomMovementMode == HEAVY_MOVEMENT_CUSTOM_Charging)
		{
			Energy -= 30.f * DeltaSeconds;
			if (Energy <= 0.f)
			{
				Energy = 0.f;
				StopMovementSpecial();
			}
		}
		else
		{
			bSpendingEnergy = false;
			AMachPlayerController* PC = Cast<AMachPlayerController>(GetController());
			if (PC)
			{
				PC->InputYawScale = PC->InputYawScale * ChargeTurnRateModifier;
				PC->InputPitchScale = PC->InputPitchScale * ChargeTurnRateModifier;
				UE_LOG(LogTemp, Warning, TEXT("Updated turn rate: %.2f"), PC->InputYawScale);
			}
		}
	}
}

void AMachHeavyCharacter::OnChargeTriggerOverlap(class AActor * OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role == ROLE_Authority && bSpendingEnergy)
	{
		AMachCharacter* Character = Cast<AMachCharacter>(OtherActor);
		if (Character && Character != this && Character->bCanBeDamaged)
		{
			FVector Dir = (Character->GetActorLocation() - GetActorLocation()).SafeNormal();
			UE_LOG(LogTemp, Warning, TEXT("Adding impulse %.2f, %.2f, %.2f"), Dir.X, Dir.Y, Dir.Z);
			Character->CharacterMovement->AddImpulse(Dir * LeapHitHorizontalImpulse + FVector(0, 0, LeapHitVerticalImuplse), true);
		}
	}
}

void AMachHeavyCharacter::StartMovementSpecial()
{
	if (!MachCharacterMovement->bIsMovementSpecialActive)
	{
		bSpendingEnergy = true;
		AMachPlayerController* PC = Cast<AMachPlayerController>(GetController());
		if (PC)
		{
			PC->InputYawScale = PC->InputYawScale / ChargeTurnRateModifier;
			PC->InputPitchScale = PC->InputPitchScale / ChargeTurnRateModifier;
			UE_LOG(LogTemp, Warning, TEXT("Updated turn rate: %.2f"), PC->InputYawScale);
		}
		Super::StartMovementSpecial();
	}
}

void AMachHeavyCharacter::StartJump()
{
	Super::StartJump();

	// Double tap jump within 0.3 seconds
	float CurrentTime = GetWorld()->GetTimeSeconds();

	UE_LOG(LogTemp, Warning, TEXT("Double jump delta %.2f"), (CurrentTime - LastJumpTime));
	if ((CurrentTime - LastJumpTime) < 0.3f && Energy >= LeapCost)
	{
		UMachHeavyCharacterMovement* HeavyMovement = Cast<UMachHeavyCharacterMovement>(MachCharacterMovement);
		if (HeavyMovement)
		{
			Energy -= LeapCost;

			bCanMove = false;
			SetThirdPerson(true);
			HeavyMovement->DoLeap(bClientUpdating);
		}
	}

	LastJumpTime = CurrentTime;
}

void AMachHeavyCharacter::LeapLanded(const FHitResult& HitResult)
{
	GetWorldTimerManager().SetTimer(this, &AMachHeavyCharacter::LeapEnded, 0.25f, false);

	static FName NAME_ApplyLeapShockwave = FName(TEXT("ApplyLeapShockwave"));
	FCollisionQueryParams SphereParams(NAME_ApplyLeapShockwave, false, this);

	SphereParams.AddIgnoredActor(this);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMulti(Overlaps, HitResult.ImpactPoint, FQuat::Identity, FCollisionShape::MakeSphere(500.f), SphereParams, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects));
	for (int32 Idx = 0; Idx < Overlaps.Num(); ++Idx)
	{
		FOverlapResult const& Overlap = Overlaps[Idx];
		AActor* const OverlapActor = Overlap.GetActor();

		if (OverlapActor && OverlapActor->bCanBeDamaged && OverlapActor != this && Overlap.Component.IsValid())
		{
			ACharacter* OverlapCharacter = Cast<ACharacter>(OverlapActor);
			if (OverlapCharacter)
			{
				FVector Dir = (OverlapCharacter->GetActorLocation() - GetActorLocation()).SafeNormal();
				UE_LOG(LogTemp, Warning, TEXT("Adding impulse %.2f, %.2f, %.2f"), Dir.X, Dir.Y, Dir.Z);
				OverlapCharacter->CharacterMovement->AddImpulse(Dir * ChargeHitHorizontalImpulse + FVector(0, 0, ChargeHitVerticalImuplse), true);
			}

			 
		}
	}
}

void AMachHeavyCharacter::LeapEnded()
{
	bCanMove = true;
	SetThirdPerson(false);
}

void AMachHeavyCharacter::OnStartFire()
{
	if (!bSpendingEnergy)
	{
		Super::OnStartFire();
	}
}

void AMachHeavyCharacter::OnReload()
{
	if (!bSpendingEnergy)
	{
		Super::OnReload();
	}
}