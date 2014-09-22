#pragma once

#include "GameFramework/HUD.h"
#include "MachMainMenuHUD.generated.h"


UCLASS()
class MACH_API AMachMainMenuHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void PlayClicked();

	UFUNCTION()
	void QuitClicked();
	
private:
	TSharedPtr<class SMachMainMenuUI> MainMenuUI;
};
