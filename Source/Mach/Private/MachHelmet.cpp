// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachHelmet.h"


// Sets default values
AMachHelmet::AMachHelmet(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMachHelmet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMachHelmet::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMachHelmet::UseVision()
{

}