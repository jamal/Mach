#include "Mach.h"
#include "MachCharacter.h"
#include "MachPlayerController.h"
#include "MachCharacterMovement.h"
#include "MachWeapon.h"
#include "MachPlayerState.h"
#include "MachGameMode.h"
#include "MachNameplateWidget.h"
#include "MachDamageTextWidget.h"
#include "UMG.h"
#include "MachHelmet.h"
#include "MachHelmetTracking.h"
#include "MachHelmetObserver.h"
#include "MachImplantKillstreak.h"

//////////////////////////////////////////////////////////////////////////
// AMachCharacter

AMachCharacter::AMachCharacter(const class FObjectInitializer& PCIP)
	: Super(PCIP.SetDefaultSubobjectClass<UMachCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	bIsDying = false;
	bCanMove = true;

	BaseHealth = 100.0f;
	BaseShield = 100.0f;
	BaseEnergy = 100.f;
	EnergyRegenRate = 10.0f;
	bSpendingEnergy = false;
	bIsBeingTracked = false;
	DamageBonusMultiplier = 0.f;

	MovementSpecialCost = 100.f;

	SniperTurnRateModifier = 0.5f;

	if (GetCharacterMovement())
	{
		MachCharacterMovement = Cast<UMachCharacterMovement>(GetCharacterMovement());
	}

	OverHeadComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("OverHeadComponent"));
	OverHeadComponent->InitSphereRadius(1.f);
	OverHeadComponent->AttachParent = GetCapsuleComponent();
	OverHeadComponent->RelativeLocation = FVector(0, 0, 128.f);

	// Create a CameraComponent	
	Camera1P = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	Camera1P->AttachParent = GetCapsuleComponent();
	Camera1P->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	Camera1P->bUsePawnControlRotation = true;
	Camera1P->bAutoActivate = true;

	HUDParticles = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("HUDParticles"));
	HUDParticles->AttachParent = GetCapsuleComponent();
	HUDParticles->RelativeLocation = FVector(32.f, 0.f, 48.f);
	HUDParticles->RelativeRotation = FRotator(0.f, -90.f, 0.f);

	ThirdPersonCameraArm = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("ThirdPersonCameraArm"));
	ThirdPersonCameraArm->TargetOffset = FVector(0.f, 0.f, 0.f);
	ThirdPersonCameraArm->SetRelativeLocation(FVector(-80.f, 0.f, 160.f));
	ThirdPersonCameraArm->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
	ThirdPersonCameraArm->AttachTo(GetCapsuleComponent()); // attach it to the third person mesh
	ThirdPersonCameraArm->TargetArmLength = 200.f;
	ThirdPersonCameraArm->bEnableCameraLag = false;
	ThirdPersonCameraArm->bEnableCameraRotationLag = false;
	ThirdPersonCameraArm->bUsePawnControlRotation = true; // let the controller handle the view rotation
	ThirdPersonCameraArm->bInheritYaw = true;
	ThirdPersonCameraArm->bInheritPitch = true;
	ThirdPersonCameraArm->bInheritRoll = false;

	Camera3P = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("ThirdPersonCamera"));
	Camera3P->AttachTo(ThirdPersonCameraArm, USpringArmComponent::SocketName);
	Camera3P->bUsePawnControlRotation = false;
	Camera3P->bAutoActivate = false;

	CameraSniper = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("SniperCamera"));
	CameraSniper->FieldOfView = 30.f;
	CameraSniper->AttachParent = GetCapsuleComponent();
	CameraSniper->RelativeLocation = FVector(64.f, 0, 48.f);
	CameraSniper->bUsePawnControlRotation = true;
	CameraSniper->bAutoActivate = false;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->AttachParent = Camera1P;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	GetMesh()->SetOwnerNoSee(true);

	WeaponAttachPoint = FName("WeaponPoint");
	static ConstructorHelpers::FObjectFinder<UBlueprint> PrimaryWeaponObject(TEXT("Blueprint'/Game/Blueprints/Weapons/Rifle.Rifle'"));
	if (PrimaryWeaponObject.Object != NULL)
	{
		DefaultPrimaryWeapon = (UClass*)PrimaryWeaponObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> SecondaryWeaponObject(TEXT("Blueprint'/Game/Blueprints/Weapons/Pistol.Pistol'"));
	if (SecondaryWeaponObject.Object != NULL)
	{
		DefaultSecondaryWeapon = (UClass*)SecondaryWeaponObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> RifleObject(TEXT("Blueprint'/Game/Blueprints/Weapons/Rifle.Rifle'"));
	if (RifleObject.Object != NULL)
	{
		Rifle = (UClass*)RifleObject.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> ShotgunObject(TEXT("Blueprint'/Game/Blueprints/Weapons/Shotgun.Shotgun'"));
	if (ShotgunObject.Object != NULL)
	{
		Shotgun = (UClass*)ShotgunObject.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> EnergyGunObject(TEXT("Blueprint'/Game/Blueprints/Weapons/EnergyGun2.EnergyGun2'"));
	if (EnergyGunObject.Object != NULL)
	{
		EnergyGun = (UClass*)EnergyGunObject.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> GrenadeLauncherObject(TEXT("Blueprint'/Game/Blueprints/Weapons/GrenadeLauncher.GrenadeLauncher'"));
	if (GrenadeLauncherObject.Object != NULL)
	{
		GrenadeLauncher = (UClass*)GrenadeLauncherObject.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> ProjCannonObject(TEXT("Blueprint'/Game/Blueprints/Weapons/ProjCannon3.ProjCannon3'"));
	if (ProjCannonObject.Object != NULL)
	{
		ProjCannon = (UClass*)ProjCannonObject.Object->GeneratedClass;
	}

	/*
	static ConstructorHelpers::FObjectFinder<UBlueprint> NameplateObject(TEXT("Blueprint'/Game/UI/Nameplate.Nameplate'"));
	if (NameplateObject.Object != NULL)
	{
		NameplateClass = (UClass*)NameplateObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> DamageTextWidgetObject(TEXT("Blueprint'/Game/UI/DamageText.DamageText'"));
	if (DamageTextWidgetObject.Object != NULL)
	{
		DamageTextWidgetClass = (UClass*)DamageTextWidgetObject.Object->GeneratedClass;
	}
	*/

	//Implant = CreateOptionalDefaultSubobject<UMachImplantComponent>(TEXT("Implant"));
	//Implant = CreateOptionalDefaultSubobject<UMachImplantRegen>(TEXT("Implant"));
	Implant = CreateOptionalDefaultSubobject<UMachImplantKillstreak>(TEXT("Implant"));
	if (Implant)
	{
		Implant->Owner = this;
	}

	LastJumpTime = 0.f;
	PrimaryActorTick.bCanEverTick = true;
}

void AMachCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// TODO: This is just test code
	if (Role == ROLE_Authority)
	{
		Pitch = GetControlRotation().Pitch;

		Health = BaseHealth;
		MaxShield = BaseShield;
		Shield = BaseShield;
		Energy = BaseEnergy;

		FActorSpawnParameters SpawnInfo;

		if (DefaultSecondaryWeapon != NULL)
		{
			SecondaryWeapon = GetWorld()->SpawnActor<AMachWeapon>(DefaultSecondaryWeapon, SpawnInfo);
		}

		if (DefaultTertiaryWeapon != NULL)
		{
			TertiaryWeapon = GetWorld()->SpawnActor<AMachWeapon>(DefaultTertiaryWeapon, SpawnInfo);
		}
	}
	else
	{
		//Mesh->bRenderCustomDepth = true;
		GetMesh()->BoundsScale = 10.0f;
	}

	//NameplateWidget = CreateWidget<UMachNameplateWidget>(GetWorld(), NameplateClass);
	//NameplateWidget->Character = this;
	//NameplateWidget->AddToViewport();

	//DamageTextWidget = CreateWidget<UMachDamageTextWidget>(GetWorld(), DamageTextWidgetClass);
	//DamageTextWidget->Damage = 0.f;
	//DamageTextWidget->DamagedActor = this;
}

void AMachCharacter::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInstance* TeamMaterial;
	if (GetTeam() == ETeam::A)
	{
		TeamMaterial = MaterialTeamA;
	}
	else
	{
		TeamMaterial = MaterialTeamB;
	}

	GetMesh()->SetMaterial(0, TeamMaterial);
	Mesh1P->SetMaterial(0, TeamMaterial);
	
	if (GetNetMode() == NM_Client)
	{
		if (NameplateWidget)
		{
			NameplateWidget->Character = this;
			NameplateWidget->AddToViewport();
			NameplateWidget->SetDesiredSizeInViewport(FVector2D(200.f, 50.f));
		}

		if (DamageTextWidget)
		{
			DamageTextWidget->Damage = 0.f;
			DamageTextWidget->DamagedActor = this;
		}
	}

	// Spawn more stuff
	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		//Helmet = GetWorld()->SpawnActor<AMachHelmetTracking>(AMachHelmetTracking::StaticClass(), Params);
		Helmet = GetWorld()->SpawnActor<AMachHelmetObserver>(AMachHelmetObserver::StaticClass(), Params);
	}
}

void AMachCharacter::Destroyed()
{
	Super::Destroyed();

	UWorld* World = GetWorld();
	if (World)
	{
		if (PrimaryWeapon)
		{
			World->DestroyActor(PrimaryWeapon);
		}
		if (SecondaryWeapon)
		{
			World->DestroyActor(SecondaryWeapon);
		}
		if (TertiaryWeapon)
		{
			World->DestroyActor(TertiaryWeapon);
		}

		if (GetNetMode() == NM_Client)
		{
			if (NameplateWidget && DamageTextWidget)
			{
				NameplateWidget->RemoveFromViewport();
				DamageTextWidget->RemoveFromViewport();

				delete NameplateWidget;
				delete DamageTextWidget;
			}
		}
	}
}

void AMachCharacter::SetPlayerDefaults()
{
	FActorSpawnParameters SpawnInfo;
	PrimaryWeapon = GetWorld()->SpawnActor<AMachWeapon>(GetPrimaryWeaponClass(), SpawnInfo);
	SetCurrentWeapon(PrimaryWeapon);
}

void AMachCharacter::UpdatePrimaryWeapon()
{
	if (PrimaryWeapon != nullptr)
	{
		PrimaryWeapon->OnUnequip();
		GetWorld()->DestroyActor(PrimaryWeapon);

		if (PrimaryWeapon == CurrentWeapon)
		{
			CurrentWeapon = nullptr;
		}

		PrimaryWeapon = nullptr;
	}

	FActorSpawnParameters SpawnInfo;
	PrimaryWeapon = GetWorld()->SpawnActor<AMachWeapon>(GetPrimaryWeaponClass(), SpawnInfo);
	SetCurrentWeapon(PrimaryWeapon);
}

TSubclassOf<class AMachWeapon> AMachCharacter::GetPrimaryWeaponClass()
{
	UE_LOG(LogTemp, Warning, TEXT("Get primary weapon"));
	AMachPlayerController* MachController = Cast<AMachPlayerController>(Controller);
	if (MachController)
	{
		switch (MachController->PlayerPawnData.PrimaryWeapon)
		{
		case EPlayerPrimaryWeapon::Shotgun:
			UE_LOG(LogTemp, Warning, TEXT("Using shotgun"));
			return Shotgun;

		case EPlayerPrimaryWeapon::EnergyGun:
			UE_LOG(LogTemp, Warning, TEXT("Using energy gun"));
			return EnergyGun;

		case EPlayerPrimaryWeapon::GrenadeLauncher:
			UE_LOG(LogTemp, Warning, TEXT("Using grenade launcher"));
			return GrenadeLauncher;

		case EPlayerPrimaryWeapon::ProjCannon:
			UE_LOG(LogTemp, Warning, TEXT("Using projection cannon"));
			return ProjCannon;

		case EPlayerPrimaryWeapon::Rifle:
		default:
			UE_LOG(LogTemp, Warning, TEXT("Using rifle"));
			return Rifle;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Returning rifle"));
	return Rifle;
}

void AMachCharacter::Tick(float DeltaSeconds)
{
	TimeSinceLastDamageTaken += DeltaSeconds;

	if (!bSpendingEnergy)
	{
		Energy = FMath::Min(Energy + EnergyRegenRate * DeltaSeconds, 100.f);
	}

	Super::Tick(DeltaSeconds);

	//MinimapTexture->UpdateResource();
}

void AMachCharacter::UpdateCameras()
{
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		if (bCamera3P)
		{
			bCamera3P = true;
			Camera1P->Deactivate();
			Camera3P->Activate();
			PC->SetViewTargetWithBlend(this, 0.2f);
			UpdateMeshes();
		}
		else
		{
			Camera1P->Activate();
			Camera3P->Deactivate();
			PC->SetViewTargetWithBlend(this, 0.2f);
			UpdateMeshes();
		}
	}
}

void AMachCharacter::UpdateMeshes()
{
	Mesh1P->SetOwnerNoSee(bCamera3P);
	GetMesh()->SetOwnerNoSee(!bCamera3P);

	if (PrimaryWeapon)
	{
		PrimaryWeapon->Mesh1P->SetOwnerNoSee(bCamera3P);
		PrimaryWeapon->Mesh3P->SetOwnerNoSee(!bCamera3P);
	}
	if (SecondaryWeapon)
	{
		SecondaryWeapon->Mesh1P->SetOwnerNoSee(bCamera3P);
		SecondaryWeapon->Mesh3P->SetOwnerNoSee(!bCamera3P);
	}
	if (TertiaryWeapon)
	{
		TertiaryWeapon->Mesh1P->SetOwnerNoSee(bCamera3P);
		TertiaryWeapon->Mesh3P->SetOwnerNoSee(!bCamera3P);
	}
}

void AMachCharacter::SetThirdPerson(bool bIsThirdPerson)
{
	bCamera3P = bIsThirdPerson;
	UpdateCameras();
	UpdateMeshes();
}

FName AMachCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

bool AMachCharacter::IsUsePressed()
{
	return bIsUsePressed;
}

bool AMachCharacter::IsFirstPerson() const
{
	return Controller && Controller->IsLocalPlayerController();
}

ETeam::Type AMachCharacter::GetTeam()
{
	AMachPlayerState* State = Cast<AMachPlayerState>(PlayerState);
	if (State)
	{
		return State->Team;
	}

	return ETeam::None;
}

void AMachCharacter::StartSprinting()
{
	if (Role < ROLE_Authority)
	{
		ServerStartSprinting();
	}

	if (MachCharacterMovement && MachCharacterMovement->CanSprint())
	{
		bIsSprinting = true;
		MachCharacterMovement->bIsSprinting = true;
	}
}

void AMachCharacter::StopSprinting()
{
	if (Role < ROLE_Authority)
	{
		ServerStopSprinting();
	}

	if (MachCharacterMovement)
	{
		bIsSprinting = false;
		MachCharacterMovement->bIsSprinting = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMachCharacter::StartJump()
{
	if (Role < ROLE_Authority)
	{
		// Double jump is only tracked on the client
		float CurrentTime = GetWorld()->GetTimeSeconds();

		if ((CurrentTime - LastJumpTime) < 0.3f)
		{
			OnDoubleJump();
			ServerDoubleJump();
		}

		LastJumpTime = CurrentTime;

		ServerOnStartJump(GetLastMovementInputVector());
	}

	bIsJumpPressed = true;

	if (bIsSprinting)
	{
		StopSprinting();
	}

	Super::Jump();
}

void AMachCharacter::StopJump()
{
	if (Role < ROLE_Authority)
	{
		ServerOnStopJump();
	}

	bIsJumpPressed = false;
}

void AMachCharacter::StartStealth()
{
	if (Role < ROLE_Authority)
	{
		ServerStartStealth();
	}
}

void AMachCharacter::OnDoubleJump()
{

}

void AMachCharacter::OnCrouchStart()
{
	bIsCrouching = true;
}

void AMachCharacter::OnCrouchStop()
{
	bIsCrouching = false;
}

void AMachCharacter::OnReload()
{
	if (CurrentWeapon != NULL)
	{
		CurrentWeapon->Reload();
	}
}

void AMachCharacter::OnStartFire()
{
	if (!bIsDying && !bFireIntent)
	{
		bFireIntent = true;
		if (CurrentWeapon != NULL)
		{
			CurrentWeapon->StartFire();
		}

		if (bIsSprinting)
		{
			StopSprinting();
		}
	}
}

void AMachCharacter::OnStopFire()
{
	if (!bIsDying && bFireIntent)
	{
		bFireIntent = 0;
		if (CurrentWeapon != NULL)
		{
			CurrentWeapon->StopFire();
		}
	}
}

void AMachCharacter::OnStartSecondaryFire()
{
	if (!bIsDying)
	{
		if (CurrentWeapon != NULL)
		{
			CurrentWeapon->StartSecondaryFire();
		}

		if (bIsSprinting)
		{
			StopSprinting();
		}
	}
}

void AMachCharacter::OnStopSecondaryFire()
{
	if (!bIsDying)
	{
		if (CurrentWeapon != NULL)
		{
			CurrentWeapon->StopSecondaryFire();
		}
	}
}

void AMachCharacter::MoveForward(float Value)
{
	if (!bIsDying && bCanMove && Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMachCharacter::MoveRight(float Value)
{
	if (!bIsDying && Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMachCharacter::StartMovementSpecial()
{
	if (!CanDoMovementSpecial())
	{
		return;
	}

	if (Role < ROLE_Authority)
	{
		ServerStartMovementSpecial();
	}
	
	if (Energy < MovementSpecialCost)
	{
		return;
	}

	Energy -= MovementSpecialCost;

	if (bIsSprinting)
	{
		StopSprinting();
	}

	if (MachCharacterMovement != NULL)
	{
		MachCharacterMovement->StartMovementSpecial(bClientUpdating);
	}
}

void AMachCharacter::StopMovementSpecial()
{
	if (Role < ROLE_Authority)
	{
		ServerStopMovementSpecial();
	}

	if (MachCharacterMovement != NULL)
	{
		MachCharacterMovement->StopMovementSpecial(bClientUpdating);
	}
}

void AMachCharacter::UseVision()
{
	if (Helmet != nullptr)
	{
		Helmet->UseVision();
	}
}

bool AMachCharacter::ServerStartMovementSpecial_Validate()
{
	return true;
}

void AMachCharacter::ServerStartMovementSpecial_Implementation()
{
	StartMovementSpecial();
}

bool AMachCharacter::ServerStopMovementSpecial_Validate()
{
	return true;
}

void AMachCharacter::ServerStopMovementSpecial_Implementation()
{
	StopMovementSpecial();
}

void AMachCharacter::SpawnNewPrimaryWeapon(TSubclassOf<class AMachWeapon> WeaponClass)
{
	if (Role < ROLE_Authority)
	{
		LocalLastWeapon = NULL;
		ServerSpawnNewPrimaryWeapon(WeaponClass);
	}
	else
	{
		if (WeaponClass != NULL)
		{
			if (PrimaryWeapon != NULL)
			{
				PrimaryWeapon->Destroy();
				PrimaryWeapon = NULL;
			}

			FActorSpawnParameters SpawnInfo;
			PrimaryWeapon = GetWorld()->SpawnActor<AMachWeapon>(WeaponClass, SpawnInfo);
			if (PrimaryWeapon) {
				UE_LOG(LogTemp, Warning, TEXT("Spawning new primary weapon: %s"), *PrimaryWeapon->GetName());
				SetCurrentWeapon(PrimaryWeapon);
			}
		}
	}
}

void AMachCharacter::SetCurrentWeapon(AMachWeapon* NewWeapon)
{
	if (Role < ROLE_Authority)
	{
		LocalLastWeapon = CurrentWeapon;
		ServerSetCurrentWeapon(NewWeapon);
	}
	else
	{
		if (NewWeapon != NULL && NewWeapon != CurrentWeapon)
		{
			NewWeapon->SetOwningPawn(this);
			NewWeapon->OnEquip();

			if (CurrentWeapon != NULL)
			{
				CurrentWeapon->OnUnequip();
			}

			CurrentWeapon = NewWeapon;
		}
	}
}

void AMachCharacter::Equip1()
{
	if (!bIsDying && PrimaryWeapon != NULL)
	{
		SetCurrentWeapon(PrimaryWeapon);
	}
}

void AMachCharacter::Equip2()
{
	if (!bIsDying && SecondaryWeapon != NULL)
	{
		SetCurrentWeapon(SecondaryWeapon);
	}
}

void AMachCharacter::Equip3()
{
	if (!bIsDying && TertiaryWeapon != NULL)
	{
		SetCurrentWeapon(TertiaryWeapon);
	}
}

void AMachCharacter::EquipLast()
{
	if (!bIsDying && LocalLastWeapon != NULL)
	{
		SetCurrentWeapon(LocalLastWeapon);
	}
}

void AMachCharacter::OnStartUse()
{
	if (Role < ROLE_Authority)
	{
		ServerOnStartUse();
	}
	else
	{
		bIsUsePressed = true;
	}
}

void AMachCharacter::OnStopUse()
{
	if (Role < ROLE_Authority)
	{
		ServerOnStopUse();
	}
	else
	{
		bIsUsePressed = false;
	}
}

bool AMachCharacter::ServerOnStartUse_Validate()
{
	return true;
}

void AMachCharacter::ServerOnStartUse_Implementation()
{
	OnStartUse();
}

bool AMachCharacter::ServerOnStopUse_Validate()
{
	return true;
}

void AMachCharacter::ServerOnStopUse_Implementation()
{
	OnStopUse();
}

bool AMachCharacter::ServerOnStartJump_Validate(FVector InputVector)
{
	return true;
}

void AMachCharacter::ServerOnStartJump_Implementation(FVector InputVector)
{
	ServerJumpMovementInput = InputVector;
	StartJump();
}

bool AMachCharacter::ServerOnStopJump_Validate()
{
	return true;
}

void AMachCharacter::ServerOnStopJump_Implementation()
{
	StopJump();
}

bool AMachCharacter::ServerStartStealth_Validate()
{
	return true;
}

void AMachCharacter::ServerStartStealth_Implementation()
{
	StartStealth();
}

void AMachCharacter::OnDeath()
{
	if (!bIsDying)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnDeath"));
		bIsDying = true;

		if (GetNetMode() == NM_Client)
		{
			NameplateWidget->RemoveFromViewport();
		}

		if (CurrentWeapon != NULL)
		{
			CurrentWeapon->StopFire();
			CurrentWeapon->OnUnequip();
		}

		Mesh1P->SetHiddenInGame(true);

		// Despawn player
		SetLifeSpan(20.0f);
	}
}

bool AMachCharacter::ShouldTakeDamage(float Damage, FDamageEvent const & DamageEvent, AController* EventInstigator, AActor* DamageCauser) const {
	if (Role == ROLE_Authority && GetWorld()->GetAuthGameMode())
	{
		AMachGameMode* GameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			return GameMode->ShouldTakeDamage(Damage, DamageEvent, EventInstigator->GetCharacter(), this);
		}
	}

	return Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AMachCharacter::ReceiveAnyDamage(float Damage, const class UDamageType* DamageType, class AController* EventInstigator, class AActor* DamageCauser)
{
	TimeSinceLastDamageTaken = 0.f;

	if (Role == ROLE_Authority && Damage != 0.f && GetWorld()->GetAuthGameMode())
	{
		AMachGameMode* GameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			Damage = GameMode->DamageAmount(Damage, EventInstigator->GetCharacter(), this);
		}

		DamageTakenCauser = DamageCauser;
		DamageTaken += Damage;

		UE_LOG(LogTemp, Warning, TEXT("Dealing %.2f damage"), Damage);

		// Instigator should get assist credit since it dealt damage to this character
		AMachPlayerState* State = Cast<AMachPlayerState>(PlayerState);
		if (State)
		{
			AMachPlayerState* InstigatorState = Cast<AMachPlayerState>(EventInstigator->PlayerState);
			if (InstigatorState)
			{
				State->AddAssistPlayer(InstigatorState);
			}
		}

		// Reset the shield regen timer every time damage is taken
		GetWorldTimerManager().SetTimer(this, &AMachCharacter::RegenerateShield, ShieldRegenStartTime, false);

		if (Shield > 0.f)
		{
			Shield = Shield - Damage;
			Damage = 0.f;
			if (Shield < 0.f)
			{
				Damage = Shield * -1;
				Shield = 0.f;
			}
		}

		if (Damage != 0.f)
		{
			Health = Health - Damage;
			if (Health <= 0.f)
			{
				Health = 0.f;
				bCanBeDamaged = false;
				GameMode->Killed(EventInstigator, Controller, this);
			}
			else if (Health >= BaseHealth)
			{
				Health = BaseHealth;
			}
		}
	}
}

void AMachCharacter::RegenerateShield()
{
	if (Role == ROLE_Authority && Shield < MaxShield)
	{
		float RateTime = 0.1f; // How often we're going to tick regen
		float Amount = ShieldRegenRate * RateTime;
		Shield = Shield + Amount;
		
		if (Shield >= MaxShield)
		{
			Shield = MaxShield;
		}
		else
		{
			GetWorldTimerManager().SetTimer(this, &AMachCharacter::RegenerateShield, RateTime, false);
		}
	}
}

bool AMachCharacter::ServerSetCurrentWeapon_Validate(AMachWeapon* NewWeapon)
{
	return true;
}

void AMachCharacter::ServerSetCurrentWeapon_Implementation(AMachWeapon* NewWeapon)
{
	SetCurrentWeapon(NewWeapon);
}

bool AMachCharacter::ServerSpawnNewPrimaryWeapon_Validate(TSubclassOf<class AMachWeapon> WeaponClass)
{
	return true;
}

void AMachCharacter::ServerSpawnNewPrimaryWeapon_Implementation(TSubclassOf<class AMachWeapon> WeaponClass)
{
	SpawnNewPrimaryWeapon(WeaponClass);
}

bool AMachCharacter::ServerStartSprinting_Validate()
{
	return true;
}

void AMachCharacter::ServerStartSprinting_Implementation()
{
	StartSprinting();
}

bool AMachCharacter::ServerStopSprinting_Validate()
{
	return true;
}

void AMachCharacter::ServerStopSprinting_Implementation()
{
	StopSprinting();
}

bool AMachCharacter::ServerDoubleJump_Validate()
{
	return true;
}

void AMachCharacter::ServerDoubleJump_Implementation()
{
	OnDoubleJump();
}

void AMachCharacter::OnRep_Health()
{
	if (Health <= 0.f)
	{
		OnDeath();
	}
}

void AMachCharacter::OnRep_CurrentWeapon(AMachWeapon* LastWeapon)
{
	if (PrimaryWeapon != NULL) PrimaryWeapon->OnUnequip();
	if (SecondaryWeapon != NULL) SecondaryWeapon->OnUnequip();

	if (CurrentWeapon != NULL)
	{
		CurrentWeapon->SetOwningPawn(this);
		CurrentWeapon->OnEquip();
	}
}

void AMachCharacter::OnRep_DamageTaken()
{
	if (DamageTaken > 0.f && GetNetMode() == NM_Client && DamageTakenCauser != nullptr)
	{
		AMachCharacter* Character = Cast<AMachCharacter>(DamageTakenCauser->Instigator);
		if (Character && Character->IsLocallyControlled())
		{
			GetWorldTimerManager().SetTimer(this, &AMachCharacter::ClearDamageTakenText, 0.5f, false);
			DamageTextWidget->Damage = DamageTaken;
			DamageTextWidget->AddToViewport();
		}
	}
}

void AMachCharacter::ClearDamageTakenText()
{
	if (GetNetMode() == NM_Client)
	{
		DamageTextWidget->Damage = 0.f;
		DamageTextWidget->RemoveFromParent();
	}
}

void AMachCharacter::OnRep_IsSprinting()
{
	if (MachCharacterMovement)
	{
		MachCharacterMovement->bIsSprinting = bIsSprinting;
	}
}

bool AMachCharacter::CanDoMovementSpecial()
{
	return true;
}

bool AMachCharacter::KillPlayer_Validate()
{
	return true;
}

void AMachCharacter::KillPlayer_Implementation()
{
	float KillDamage = (BaseHealth + BaseShield) * 2.f; // Need to double it since it's "friendly" damage

	FPointDamageEvent PointDmg;
	PointDmg.Damage = KillDamage;

	TakeDamage(KillDamage, PointDmg, Controller, this);
}

float AMachCharacter::GetHealthPercent()
{
	return Health / BaseHealth;
}

float AMachCharacter::GetShieldPercent()
{
	return Shield / MaxShield;
}

float AMachCharacter::GetEnergyPercent()
{
	return Energy / BaseEnergy;
}

void AMachCharacter::SetShieldMultiplier(float Multiplier)
{
	MaxShield = BaseShield * Multiplier;
	if (Shield < MaxShield)
	{
		GetWorldTimerManager().SetTimer(this, &AMachCharacter::RegenerateShield, ShieldRegenStartTime, false);
	}
}

void AMachCharacter::UpdateShieldMultiplier()
{
	AMachGameMode* GameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		SetShieldMultiplier(GameMode->ShieldMultiplier(this));
	}
}

void AMachCharacter::OnTechPointCaptured()
{
	UpdateShieldMultiplier();
}

void AMachCharacter::OnTechPointLost()
{
	UpdateShieldMultiplier();
}

void AMachCharacter::SetSniperCamera(bool bEnabled)
{
	bIsSniperCameraEnabled = bEnabled;

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		if (bEnabled)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enabled sniper camera"));
			Camera1P->Deactivate();
			Camera1P->bUsePawnControlRotation = false;
			Camera3P->Deactivate();
			Camera3P->bUsePawnControlRotation = false;
			CameraSniper->Activate();
			CameraSniper->bUsePawnControlRotation = true;
			PC->SetViewTargetWithBlend(this, 1.f);

			MachCharacterMovement->bIsMovementLimited = true;
			AMachPlayerController* PC = Cast<AMachPlayerController>(GetController());
			if (PC)
			{
				PC->InputYawScale = PC->InputYawScale * SniperTurnRateModifier;
				PC->InputPitchScale = PC->InputPitchScale * SniperTurnRateModifier;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Disabled sniper camera"));
			Camera1P->Activate();
			Camera1P->bUsePawnControlRotation = true;
			Camera3P->Deactivate();
			Camera3P->bUsePawnControlRotation = false;
			CameraSniper->Deactivate();
			CameraSniper->bUsePawnControlRotation = false;
			PC->SetViewTargetWithBlend(this, 1.f);

			MachCharacterMovement->bIsMovementLimited = false;
			AMachPlayerController* PC = Cast<AMachPlayerController>(GetController());
			if (PC)
			{
				PC->InputYawScale = PC->InputYawScale / SniperTurnRateModifier;
				PC->InputPitchScale = PC->InputPitchScale / SniperTurnRateModifier;
			}
		}
	}
}

void AMachCharacter::StartTracker()
{
	// This should only be called on the server
	if (Role == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start tracker"));
		if (!bIsBeingTracked)
		{
			bIsBeingTracked = true;
			GetWorldTimerManager().SetTimer(this, &AMachCharacter::StopTracker, 30.f, false);
		}
	}
}

void AMachCharacter::StopTracker()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop tracker"));
	GetMesh()->bRenderCustomDepth = false;
	bIsBeingTracked = false;
}

void AMachCharacter::OnRep_IsBeingTracked()
{
	if (Role < ROLE_Authority)
	{
		// Short circuit if we're stopping
		if (!bIsBeingTracked && GetMesh()->bRenderCustomDepth)
		{
			GetMesh()->SetRenderCustomDepth(false);
		}
		else
		{
			// TODO: This is probably not the best approach, but we'll address it at a later time
			AMachPlayerController* Controller = Cast<AMachPlayerController>(GetWorld()->GetFirstPlayerController());
			if (Controller && Controller->IsLocalController() && GetTeam() != Controller->GetTeam()) {
				GetMesh()->SetRenderCustomDepth(bIsBeingTracked);
			}
		}
	}
}

void AMachCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachCharacter, bIsUsePressed);
	DOREPLIFETIME(AMachCharacter, bIsJumpPressed);
	DOREPLIFETIME(AMachCharacter, bIsDying);
	DOREPLIFETIME(AMachCharacter, bIsCrouching);
	DOREPLIFETIME(AMachCharacter, Health);
	DOREPLIFETIME(AMachCharacter, Shield);
	DOREPLIFETIME(AMachCharacter, MaxShield);
	DOREPLIFETIME(AMachCharacter, Energy);
	DOREPLIFETIME(AMachCharacter, CurrentWeapon);
	DOREPLIFETIME(AMachCharacter, PrimaryWeapon);
	DOREPLIFETIME(AMachCharacter, SecondaryWeapon);
	DOREPLIFETIME(AMachCharacter, TertiaryWeapon);
	DOREPLIFETIME(AMachCharacter, Helmet);
	DOREPLIFETIME(AMachCharacter, bIsBeingTracked);
	DOREPLIFETIME(AMachCharacter, bIsVisibleToEnemy);
	DOREPLIFETIME(AMachCharacter, Killstreak);
	DOREPLIFETIME(AMachCharacter, DamageBonusMultiplier);
	DOREPLIFETIME_CONDITION(AMachCharacter, bIsSprinting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMachCharacter, bSpendingEnergy, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMachCharacter, Pitch, COND_SkipOwner);
	DOREPLIFETIME(AMachCharacter, DamageTaken);
	DOREPLIFETIME(AMachCharacter, DamageTakenCauser);
}