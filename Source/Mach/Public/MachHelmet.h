// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "MachHelmet.generated.h"

UCLASS()
class MACH_API AMachHelmet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMachHelmet(const class FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void UseVision();
	
};
