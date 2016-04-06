#include "Mach.h"
#include "MachPlayerController.h"
#include "MachCharacter.h"
#include "MachGameMode.h"
#include "MachHUD.h"
#include "MachPlayerState.h"

AMachPlayerController::AMachPlayerController(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	NAME_Say = FName(TEXT("Say"));
	NAME_TeamSay = FName(TEXT("TeamSay"));

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bEquipmentPicked = true;

	// TODO: PlayerController may not be the best place to do this, so yeah, think about that
	static ConstructorHelpers::FObjectFinder<UBlueprint> PreviewActorObject(TEXT("Blueprint'/Game/Blueprints/PreviewActor.PreviewActor'"));
	if (PreviewActorObject.Object != NULL)
	{
		PreviewActorClass = (UClass*)PreviewActorObject.Object->GeneratedClass;
	}

	// Set some default spawn data, this is just for dev / editor when we don't go through login
	PlayerPawnData.Armor = EPlayerArmor::Medium;
	PlayerPawnData.PrimaryWeapon = EPlayerPrimaryWeapon::Rifle;
}

void AMachPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

AMachCharacter* AMachPlayerController::GetMachCharacter()
{
	return Cast<AMachCharacter>(GetCharacter());
}

void AMachPlayerController::SetIgnoreInput(bool bInput)
{
	bIgnoreInput = bInput;
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

	// Movement
	InputComponent->BindAxis("MoveForward", this, &AMachPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMachPlayerController::MoveRight);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AMachPlayerController::Crouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &AMachPlayerController::UnCrouch);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AMachPlayerController::StartSprinting);
	InputComponent->BindAction("Sprint", IE_Released, this, &AMachPlayerController::StopSprinting);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AMachPlayerController::StartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &AMachPlayerController::StopJump);
	InputComponent->BindAction("MovementSpecial", IE_Pressed, this, &AMachPlayerController::StartMovementSpecial);
	InputComponent->BindAction("MovementSpecial", IE_Released, this, &AMachPlayerController::StopMovementSpecial);
	InputComponent->BindAction("Stealth", IE_Pressed, this, &AMachPlayerController::StartStealth);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AMachPlayerController::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AMachPlayerController::OnStopFire);
	InputComponent->BindAction("SecondaryFire", IE_Pressed, this, &AMachPlayerController::OnStartSecondaryFire);
	InputComponent->BindAction("SecondaryFire", IE_Released, this, &AMachPlayerController::OnStopSecondaryFire);
	InputComponent->BindAction("Equip1", IE_Pressed, this, &AMachPlayerController::Equip1);
	InputComponent->BindAction("Equip2", IE_Pressed, this, &AMachPlayerController::Equip2);
	InputComponent->BindAction("Equip3", IE_Pressed, this, &AMachPlayerController::Equip3);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AMachPlayerController::OnReload);

	InputComponent->BindAction("Use", IE_Pressed, this, &AMachPlayerController::OnStartUse);
	InputComponent->BindAction("Use", IE_Released, this, &AMachPlayerController::OnStopUse);

	InputComponent->BindAction("UseVision", IE_Pressed, this, &AMachPlayerController::UseVision);

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

void AMachPlayerController::MoveForward(float Value)
{
	if (GetMachCharacter() != NULL)
	{
		GetMachCharacter()->MoveForward(Value);
	}
}

void AMachPlayerController::MoveRight(float Value)
{
	if (GetMachCharacter() != NULL)
	{
		GetMachCharacter()->MoveRight(Value);
	}
}

void AMachPlayerController::StartJump()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->StartJump();
	}
}

void AMachPlayerController::StopJump()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->StopJump();
	}
}

void AMachPlayerController::StartMovementSpecial()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->StartMovementSpecial();
	}
}

void AMachPlayerController::StopMovementSpecial()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->StopMovementSpecial();
	}
}

void AMachPlayerController::OnStartFire()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->OnStartFire();
	}
}

void AMachPlayerController::OnStopFire()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->OnStopFire();
	}
}

void AMachPlayerController::OnStartSecondaryFire()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->OnStartSecondaryFire();
	}
}

void AMachPlayerController::OnStopSecondaryFire()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->OnStopSecondaryFire();
	}
}

void AMachPlayerController::Equip1()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->Equip1();
	}
}

void AMachPlayerController::Equip2()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->Equip2();
	}
}

void AMachPlayerController::Equip3()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->Equip3();
	}
}

void AMachPlayerController::OnReload()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->OnReload();
	}
}

void AMachPlayerController::OnStartUse()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->OnStartUse();
	}
}

void AMachPlayerController::OnStopUse()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->OnStopUse();
	}
}

void AMachPlayerController::Crouch()
{
	if (!bIgnoreInput && GetCharacter() != NULL)
	{
		GetCharacter()->Crouch();
	}
}

void AMachPlayerController::UnCrouch()
{
	if (!bIgnoreInput && GetCharacter() != NULL)
	{
		GetCharacter()->UnCrouch();
	}
}

void AMachPlayerController::StartSprinting()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->StartSprinting();
	}
}

void AMachPlayerController::StopSprinting()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->StopSprinting();
	}
}

void AMachPlayerController::StartStealth()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->StartStealth();
	}
}

void AMachPlayerController::UseVision()
{
	if (!bIgnoreInput && GetMachCharacter() != NULL)
	{
		GetMachCharacter()->UseVision();
	}
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

bool AMachPlayerController::SetPlayerData_Validate(FPlayerData NewPlayerData)
{
	return true;
}

void AMachPlayerController::SetPlayerData_Implementation(FPlayerData NewPlayerData)
{
	bEquipmentPicked = true;
	PlayerPawnData = NewPlayerData;
}

bool AMachPlayerController::CanRestartPlayer()
{
	if (!bEquipmentPicked)
	{
		return false;
	}

	return Super::CanRestartPlayer();
}

void AMachPlayerController::PawnPendingDestroy(APawn* Pawn)
{
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	FRotator ViewDir = GetControlRotation();
	ViewDir.Pitch = -45.0f;
	FVector CameraLocation = PawnLocation + FVector(0, 0, 300.0f);
	FRotator CameraRotation(-90.0f, 0.0f, 0.0f);

	// Try to find an angle where we can see the body without the camera being blocked
	const float YawOffsets[] = { 0.0f, -180.0f, 90.0f, -90.0f, 45.0f, -45.0f, 135.0f, -135.0f };
	const float CameraOffset = 1200.0f;
	FCollisionQueryParams TraceParams(TEXT("DeathCamera"), true, GetPawn());

	for (int32 i = 0; i < ARRAY_COUNT(YawOffsets); i++)
	{
		FRotator CameraDir = ViewDir;
		CameraDir.Yaw += YawOffsets[i];
		CameraDir.Normalize();

		const FVector TestLocation = PawnLocation - CameraDir.Vector() * CameraOffset;
		const bool bBlocked = GetWorld()->LineTraceTest(PawnLocation, TestLocation, ECC_Camera, TraceParams);

		if (!bBlocked)
		{
			CameraLocation = TestLocation;
			CameraRotation = CameraDir;
		}
	}

	// Now we can continue destroying the pawn
	Super::PawnPendingDestroy(Pawn);

	ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

void AMachPlayerController::ClientSetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation)
{
	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
	SetViewTarget(this);
}

void AMachPlayerController::LightArmor()
{
	SwapArmor(EPlayerArmor::Light);
}

void AMachPlayerController::MediumArmor()
{
	SwapArmor(EPlayerArmor::Medium);
}

void AMachPlayerController::StealthArmor()
{
	SwapArmor(EPlayerArmor::Stealth);
}

void AMachPlayerController::HeavyArmor()
{
	SwapArmor(EPlayerArmor::Heavy);
}

bool AMachPlayerController::SwapArmor_Validate(EPlayerArmor::Type NewArmor)
{
	return true;
}

void AMachPlayerController::SwapArmor_Implementation(EPlayerArmor::Type NewArmor)
{
	// TODO: Verify AllowCheats is on
	PlayerPawnData.Armor = NewArmor;
	AMachGameMode* GameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->SwapPawn(this);
	}
}

bool AMachPlayerController::SendMessage_Validate(const FString& S)
{
	return true;
}

void AMachPlayerController::SendMessage_Implementation(const FString& S)
{
	AGameMode* GameMode = GetWorld()->GetAuthGameMode();
	if (GameMode)
	{
		GameMode->Broadcast(GetCharacter(), S, NAME_Say);
	}
}

void AMachPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime)
{
	FString SMod = S;
	if (SenderPlayerState != NULL)
	{
		if (Type == NAME_Say)
		{
			SMod = FString::Printf(TEXT("%s: %s"), *SenderPlayerState->PlayerName, *SMod);
		}
		else if (Type == NAME_TeamSay)
		{
			SMod = FString::Printf(TEXT("%s (team): %s"), *SenderPlayerState->PlayerName, *SMod);
		}
	}

	AMachHUD* MachHUD = Cast<AMachHUD>(GetHUD());
	if (MachHUD)
	{
		MachHUD->AddChatLine(SMod, MsgLifeTime);
		OnClientMessage(SenderPlayerState, S, Type, MsgLifeTime);
	}
}

void AMachPlayerController::Rifle()
{
	SwapWeapon(EPlayerPrimaryWeapon::Rifle);
}

void AMachPlayerController::Shotgun()
{
	SwapWeapon(EPlayerPrimaryWeapon::Shotgun);
}

void AMachPlayerController::GrenadeLauncher()
{
	SwapWeapon(EPlayerPrimaryWeapon::GrenadeLauncher);
}

void AMachPlayerController::ProjCannon()
{
	SwapWeapon(EPlayerPrimaryWeapon::ProjCannon);
}

bool AMachPlayerController::SwapWeapon_Validate(EPlayerPrimaryWeapon::Type NewArmor)
{
	return true;
}

void AMachPlayerController::SwapWeapon_Implementation(EPlayerPrimaryWeapon::Type NewWeapon)
{
	// TODO: Verify AllowCheats is on
	PlayerPawnData.PrimaryWeapon = NewWeapon;
	GetMachCharacter()->UpdatePrimaryWeapon();
}

void AMachPlayerController::SetInputModeGameAndUI()
{
	UE_LOG(LogTemp, Warning, TEXT("Switching input to game and UI"));
	SetIgnoreLookInput(true);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewport(true);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	
	int32 SizeX, SizeY;
	GetViewportSize(SizeX, SizeY);
	SetMousePosition(SizeX / 2, SizeY / 2);
}

void AMachPlayerController::SetInputModeGameOnly()
{
	UE_LOG(LogTemp, Warning, TEXT("Switching input to game only"));
	SetIgnoreLookInput(false);

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

ETeam::Type AMachPlayerController::GetTeam()
{
	AMachPlayerState* State = Cast<AMachPlayerState>(PlayerState);
	if (State)
	{
		return State->Team;
	}

	return ETeam::None;
}
