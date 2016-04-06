// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachBuildable.h"
#include "MachWeaponProjCannon.h"

AMachBuildable::AMachBuildable(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bIsActive = false;
	bIsDeployed = false;
	Range = 500.f;
	SetActorEnableCollision(false);

	RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->AttachParent = RootComponent;

	// TODO: Fuck this implementation hard
	Health = 100.f;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GoodMaterialObject(TEXT("Material'/Game/Materials/Weapons/M_Buildable_Good.M_Buildable_Good'"));
	if (GoodMaterialObject.Object != NULL)
	{
		UMaterialInterface* GoodMaterial = (UMaterialInterface*)GoodMaterialObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BadMaterialObject(TEXT("Material'/Game/Materials/Weapons/M_Buildable_Bad.M_Buildable_Bad'"));
	if (BadMaterialObject.Object != NULL)
	{
		UMaterialInterface* BadMaterial = (UMaterialInterface*)BadMaterialObject.Object;
	}
}

bool AMachBuildable::SetPreviewLocation(const FHitResult &Hit)
{
	if (bIsDeployed)
	{
		return false;
	}

	if (Hit.bBlockingHit)
	{
		SetActorHiddenInGame(false);
		SetActorLocationAndRotation(Hit.ImpactPoint, Hit.ImpactNormal.Rotation() + FRotator(-90.f, 0, 0));

		if (OriginalMaterial == nullptr)
		{
			OriginalMaterial = Mesh->GetMaterial(0);
		}

		if (CanBeDeployed())
		{
			Mesh->SetMaterial(0, GoodMaterial);
			return true;
		}
		else
		{
			Mesh->SetMaterial(0, BadMaterial);
			return false;
		}
	}

	SetActorHiddenInGame(true);
	return false;
}

bool AMachBuildable::CanBeDeployed()
{
	return CanBeDeployed(GetActorLocation(), GetActorRotation());
}

bool AMachBuildable::CanBeDeployed(FVector Location, FRotator Rotation)
{
	const float Distance = (GetInstigator()->GetActorLocation() - Location).Size();
	if (Distance <= Range)
	{
		float Dot = FVector::DotProduct(Rotation.Vector().SafeNormal(), FVector::ForwardVector);
		return Dot == 1.0f;
	}

	return false;
}

bool AMachBuildable::Deploy()
{
	if (CanBeDeployed())
	{
		bIsActive = true;
		bIsDeployed = true;
		SetActorEnableCollision(true);
		Mesh->SetMaterial(0, OriginalMaterial);
		return true;
	}

	return false;
}

void AMachBuildable::Destroyed()
{
	AMachWeaponProjCannon* ProjCannon = Cast<AMachWeaponProjCannon>(GetOwner());
	if (ProjCannon)
	{
		ProjCannon->OnBuildableDestroyed(this);
	}

	Super::Destroyed();
}

void AMachBuildable::ReceiveAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser)
{
	Health -= Damage;
	if (Health <= 0.f)
	{
		bIsActive = false;
		SetLifeSpan(1.0f);
	}
	UE_LOG(LogTemp, Warning, TEXT("Health %.2f"), Health);
}
