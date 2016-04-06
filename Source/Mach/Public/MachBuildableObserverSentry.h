// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachBuildable.h"
#include "MachBuildableObserverSentry.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachBuildableObserverSentry : public AMachBuildable
{
	GENERATED_BODY()
	
public:

	AMachBuildableObserverSentry(const class FObjectInitializer& ObjectInitializer);
	
	virtual void Tick(float DeltaTime) override;

};
