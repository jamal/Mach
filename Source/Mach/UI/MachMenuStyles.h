#pragma once

/**
 * 
 */
class MACH_API FMachMenuStyles
{
public:
	static void Initialize();

	static void Shutdown();

	static const class ISlateStyle& Get();

	static FName GetStyleSetName();

private:
	static TSharedPtr<class FSlateStyleSet> MenuStyleInstance;

	static TSharedRef<class FSlateStyleSet> Create();
};
