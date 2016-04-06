// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachBuildableObserverSentry.h"

AMachBuildableObserverSentry::AMachBuildableObserverSentry(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMachBuildableObserverSentry::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Tick"));
}