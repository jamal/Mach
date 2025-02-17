// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachImplantComponent.h"


// Sets default values for this component's properties
UMachImplantComponent::UMachImplantComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMachImplantComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// ...
	
}


// Called every frame
void UMachImplantComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

}

