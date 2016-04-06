// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachTurret.h"
#include "MachCharacter.h"
#include "MachPlayerState.h"

AMachTurret::AMachTurret(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bReplicates = true;
	FireRange = 2000.f;
	FireRate = 0.5f;
	Damage = 5.f;

	PrimaryActorTick.bCanEverTick = true;

	BuildableType = EProjCannonBuildable::Turret;
	
	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("TurretMesh"));
	Mesh->AttachParent = RootComponent;

	SphereComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(FireRange);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AMachTurret::OnBeginOverlap);
 	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AMachTurret::OnEndOverlap);
	SphereComponent->AttachParent = RootComponent;
}

void AMachTurret::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role == ROLE_Authority && OtherActor && OtherActor != this && OtherComp && OtherActor->bCanBeDamaged)
	{
		// TODO: What about supporting other non-character actors?
		AMachCharacter* MachCharacter = Cast<AMachCharacter>(OtherActor);
		if (MachCharacter)
		{
			AMachPlayerState* PS = Cast<AMachPlayerState>(MachCharacter->PlayerState);
			if (PS && PS->Team != Team)
			{
				UE_LOG(LogTemp, Warning, TEXT("Adding actor to turret range"));
				ActorsInRange.AddUnique(OtherActor);

				if (ActorsInRange.Num() == 1)
				{
					GetWorldTimerManager().SetTimer(this, &AMachTurret::Fire, FireRate, true);
				}
			}
		}
	}
}

void AMachTurret::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("AMachTurret::OnEndOverlap"));
	if (Role == ROLE_Authority && OtherActor && OtherActor != this && OtherComp && OtherActor->bCanBeDamaged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Removing actor from turret range"));
		ActorsInRange.Remove(OtherActor);

		if (ActorsInRange.Num() == 0)
		{
			GetWorldTimerManager().ClearTimer(this, &AMachTurret::Fire);
		}
	}
}

void AMachTurret::Fire()
{
	for (AActor* Actor : ActorsInRange)
	{
		static FName NAME_TurretLOS = FName(TEXT("TurretLOS"));
		FCollisionQueryParams CollisionParms(NAME_TurretLOS, true, Actor);
		CollisionParms.AddIgnoredActor(this);
		FVector TargetLocation = Actor->GetTargetLocation(this);
		bool bHit = GetWorld()->LineTraceTest(GetActorLocation() + FVector(0, 0, 10.f), TargetLocation, ECC_Visibility, CollisionParms);
		if (!bHit)
		{
			CurrentTarget = Actor;
			FPointDamageEvent PointDmg;
			PointDmg.Damage = Damage;
			CurrentTarget->TakeDamage(Damage, PointDmg, GetInstigator()->Controller, this);

			break;
		}
	}
}

void AMachTurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachTurret, CurrentTarget);
}
