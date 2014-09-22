

#include "Mach.h"
#include "MachMainMenuController.h"


AMachMainMenuController::AMachMainMenuController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Enable the mouse on the main menu
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}


