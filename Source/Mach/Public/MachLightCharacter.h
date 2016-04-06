// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "MachCharacter.h"
#include "MachLightCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachLightCharacter : public AMachCharacter
{
	GENERATED_UCLASS_BODY()
	
public:
	/** Number of jumps performed since last landing. */
	int32 Jumps;

protected:
	bool CanJumpInternal_Implementation() const;
	void OnJumped_Implementation();
	void OnLanded(const FHitResult& Hit);
};
