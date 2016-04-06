// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachHelmetObserver.h"

AMachHelmetObserver::AMachHelmetObserver(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootScene"));
}

// Called when the game starts or when spawned
void AMachHelmetObserver::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMachHelmetObserver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMachHelmetObserver::UseVision()
{
	UE_LOG(LogTemp, Warning, TEXT("Drop the thing"));
}