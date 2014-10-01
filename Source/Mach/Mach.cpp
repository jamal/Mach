// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Mach.h"

class FMachGameModule : public FDefaultGameModuleImpl
{
#if 0
	virtual void StartupModule() override
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(FMachMenuStyles::GetStyleSetName());
		FMachMenuStyles::Initialize();
	}

	virtual void ShutdownModule() override
	{
		FMachMenuStyles::Shutdown();
	}
#endif // 0

};

IMPLEMENT_PRIMARY_GAME_MODULE(FMachGameModule, Mach, "Mach");
 