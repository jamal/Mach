// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachVendbot.h"
#include "MachCharacter.h"
#include "MachPlayerState.h"

AMachVendbot::AMachVendbot(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bReplicates = true;
	VendbotRange = 1000.f;
	Rate = 0.1f;
	HealthPerTick = 1.0f;

	BuildableType = EProjCannonBuildable::Vendbot;

	RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));

	Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("VendbotMesh"));
	Mesh->AttachParent = RootComponent;

	SphereComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(VendbotRange);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AMachVendbot::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AMachVendbot::OnEndOverlap);
	SphereComponent->AttachParent = RootComponent;
}

void AMachVendbot::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role == ROLE_Authority && OtherActor && OtherActor != this && OtherComp && OtherActor->bCanBeDamaged)
	{
		AMachCharacter* MachCharacter = Cast<AMachCharacter>(OtherActor);
		if (MachCharacter)
		{
			AMachPlayerState* PS = Cast<AMachPlayerState>(MachCharacter->PlayerState);
			if (PS && PS->Team == Team)
			{
				UE_LOG(LogTemp, Warning, TEXT("Adding actor to vendbot range"));
				ActorsInRange.AddUnique(OtherActor);

				if (ActorsInRange.Num() == 1)
				{
					GetWorldTimerManager().SetTimer(this, &AMachVendbot::Regen, Rate, true);
				}
			}
		}
	}
}

void AMachVendbot::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority && OtherActor && OtherActor != this && OtherComp && OtherActor->bCanBeDamaged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Removing actor from vendbot range"));
		ActorsInRange.Remove(OtherActor);

		if (ActorsInRange.Num() == 0)
		{
			GetWorldTimerManager().ClearTimer(this, &AMachVendbot::Regen);
		}
	}
}

void AMachVendbot::Regen()
{
	for (AActor* Actor : ActorsInRange)
	{
		static FName NAME_VendbotLOS = FName(TEXT("VendbotLOS"));
		FCollisionQueryParams CollisionParms(NAME_VendbotLOS, true, Actor);
		CollisionParms.AddIgnoredActor(this);
		FVector TargetLocation = Actor->GetTargetLocation(this);
		bool bHit = GetWorld()->LineTraceTest(GetActorLocation() + FVector(0, 0, 10.f), TargetLocation, ECC_Visibility, CollisionParms);
		if (!bHit && Actor->bCanBeDamaged)
		{
			FPointDamageEvent PointDmg;
			PointDmg.Damage = HealthPerTick * -1;
			Actor->TakeDamage(HealthPerTick * -1, PointDmg, GetInstigator()->Controller, this);
		}
	}
}