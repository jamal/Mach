// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachGameInstance.h"
#include "MachLocalPlayer.h"


UMachLocalPlayer* UMachGameInstance::GetFirstMachPlayer()
{
	UMachLocalPlayer* MLP = Cast<UMachLocalPlayer>(GetFirstGamePlayer());
	if (MLP)
	{
		return MLP;
	}
	return nullptr;
}
