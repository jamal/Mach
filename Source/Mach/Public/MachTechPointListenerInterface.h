// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachTechPointListenerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMachTechPointListenerInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IMachTechPointListenerInterface
{
	GENERATED_IINTERFACE_BODY()
    
    virtual void OnTechPointCaptured() = 0;
    virtual void OnTechPointLost() = 0;
};