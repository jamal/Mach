#include "Mach.h"
#include "MachMainMenuGameMode.h"
#include "MachMainMenuHUD.h"
#include "MachMainMenuController.h"


AMachMainMenuGameMode::AMachMainMenuGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	HUDClass = AMachMainMenuHUD::StaticClass();
	PlayerControllerClass = AMachMainMenuController::StaticClass();
}
