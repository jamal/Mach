// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Mach.h"
#include "Slate.h"
#include "UI/MachMenuStyles.h"
#include "Mach.generated.inl"

class FMachGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(FMachMenuStyles::GetStyleSetName());
		FMachMenuStyles::Initialize();
	}

	virtual void ShutdownModule() override
	{
		FMachMenuStyles::Shutdown();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FMachGameModule, Mach, "Mach");
 