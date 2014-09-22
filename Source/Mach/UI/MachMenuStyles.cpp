#include "Mach.h"
#include "MachMenuStyles.h"
#include "Slate.h"
#include "SlateGameResources.h"


TSharedPtr<FSlateStyleSet> FMachMenuStyles::MenuStyleInstance;

void FMachMenuStyles::Initialize()
{
	if (!MenuStyleInstance.IsValid())
	{
		MenuStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*MenuStyleInstance);
	}
}

void FMachMenuStyles::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*MenuStyleInstance);
	MenuStyleInstance.Reset();
}

FName FMachMenuStyles::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MenuStyles"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FMachMenuStyles::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(FMachMenuStyles::GetStyleSetName(), "/Game/UI/Styles", "/Game/UI/Styles");
	return StyleRef;
}

const ISlateStyle& FMachMenuStyles::Get()
{
	return *MenuStyleInstance;
}