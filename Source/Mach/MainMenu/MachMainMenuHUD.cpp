#include "Mach.h"
#include "MachMainMenuHUD.h"
#include "MachMainMenuUI.h"


AMachMainMenuHUD::AMachMainMenuHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

void AMachMainMenuHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GEngine && GEngine->GameViewport)
	{
		UGameViewportClient* Viewport = GEngine->GameViewport;

		SAssignNew(MainMenuUI, SMachMainMenuUI)
			.MenuHUD(TWeakObjectPtr<AMachMainMenuHUD>(this));

		Viewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(MainMenuUI.ToSharedRef()));
	}
}

void AMachMainMenuHUD::PlayClicked()
{
}

void AMachMainMenuHUD::QuitClicked()
{
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}