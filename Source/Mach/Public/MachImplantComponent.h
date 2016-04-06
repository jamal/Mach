// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "MachCharacter.h"
#include "MachImplantComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACH_API UMachImplantComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMachImplantComponent();

	AMachCharacter* Owner;

	// Called when the game starts
	virtual void InitializeComponent() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

};
