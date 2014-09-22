#pragma once

#include "Slate.h"

/**
 * 
 */
class MACH_API SMachMainMenuUI : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMachMainMenuUI)
		: _MenuHUD()
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<class AMachMainMenuHUD>, MenuHUD);

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<class AMachMainMenuHUD> MenuHUD;

	FReply PlayClicked();
	FReply QuitClicked();
};
