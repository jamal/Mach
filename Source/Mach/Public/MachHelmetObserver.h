// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachHelmet.h"
#include "MachHelmetObserver.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachHelmetObserver : public AMachHelmet
{
	GENERATED_BODY()

public:
	AMachHelmetObserver(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void UseVision() override;
		
};
