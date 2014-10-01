#include "Mach.h"
#include "MachPlayerController.h"


AMachPlayerController::AMachPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

void AMachPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// UI Input
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &AMachPlayerController::OnShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &AMachPlayerController::OnHideScoreboard);
}

AMachHUD* AMachPlayerController::GetMachHUD() const
{
	return Cast<AMachHUD>(GetHUD());
}

void AMachPlayerController::OnShowScoreboard()
{
	AMachHUD* MachHUD = GetMachHUD();
	if (MachHUD)
	{
		MachHUD->ShowScoreboard(true);
	}
}

void AMachPlayerController::OnHideScoreboard()
{
	AMachHUD* MachHUD = GetMachHUD();
	if (MachHUD)
	{
		MachHUD->ShowScoreboard(false);
	}
}