#include "Mach.h"


AMachMainMenuGameMode::AMachMainMenuGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	HUDClass = AMachMainMenuHUD::StaticClass();
	PlayerControllerClass = AMachMainMenuController::StaticClass();
}
