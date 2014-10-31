#include "Mach.h"
#include "MachPlayerController.h"


AMachPlayerController::AMachPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

void AMachPlayerController::SetIgnoreInput(bool bInput)
{
	SetIgnoreLookInput(bInput);
	SetIgnoreMoveInput(bInput);
}

void AMachPlayerController::SetMousePosition(int32 x, int32 y)
{
	FViewport* v = CastChecked<ULocalPlayer>(Player)->ViewportClient->Viewport;
	v->SetMouse(x, y);
}

void AMachPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Mouse Look
	InputComponent->BindAxis("Turn", this, &APlayerController::AddYawInput);
	InputComponent->BindAxis("TurnRate", this, &AMachPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APlayerController::AddPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AMachPlayerController::LookUpAtRate);

	// UI Input
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &AMachPlayerController::OnShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &AMachPlayerController::OnHideScoreboard);
}

void AMachPlayerController::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMachPlayerController::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
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