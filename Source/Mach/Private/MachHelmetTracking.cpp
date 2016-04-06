// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachCharacter.h"
#include "MachHelmetTracking.h"

// Sets default values
AMachHelmetTracking::AMachHelmetTracking(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootScene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootScene"));
	RootComponent = RootScene;

	Range = 12000.f;
}

// Called when the game starts or when spawned
void AMachHelmetTracking::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMachHelmetTracking::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMachHelmetTracking::UseVision()
{
	AController* Controller = Instigator ? Instigator->Controller : nullptr;
	if (Controller)
	{
		FVector Start, End;
		FRotator Rot;
		Controller->GetPlayerViewPoint(Start, Rot);
		End = Start + Range * Rot.Vector();

		FCollisionQueryParams TraceParams(FName(TEXT("HelmetTrackingTrace")), true, Instigator);
		TraceParams.bTraceAsyncScene = true;

		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingle(Hit, Start, End, ECC_Pawn, TraceParams);

		if (Hit.bBlockingHit && Hit.Actor != nullptr)
		{
			AMachCharacter* Character = Cast<AMachCharacter>(Hit.Actor.Get());
			AMachCharacter* OwnerCharacter = Cast<AMachCharacter>(GetOwner());
			if (Character && OwnerCharacter && Character->GetTeam() != OwnerCharacter->GetTeam())
			{
				Character->StartTracker();
				ServerTrackingHit(Hit);
			}
		}
	}
}

bool AMachHelmetTracking::ServerTrackingHit_Validate(const FHitResult& Hit)
{
	return true;
}

void AMachHelmetTracking::ServerTrackingHit_Implementation(const FHitResult& Hit)
{
	if (Hit.bBlockingHit)
	{
		AMachCharacter* Character = Cast<AMachCharacter>(Hit.Actor.Get());
		AMachCharacter* OwnerCharacter = Cast<AMachCharacter>(GetOwner());
		if (Character && OwnerCharacter && Character->GetTeam() != OwnerCharacter->GetTeam())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit server"));
			Character->StartTracker();
		}
	}
}