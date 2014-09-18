#include "Mach.h"
#include "MachCharacter.h"
#include "MachProjectile.h"
#include "MachWeapon.h"
#include "MachPlayerState.h"
#include "MachUseTrigger.h"
#include "MachGameMode.h"


//////////////////////////////////////////////////////////////////////////
// AMachCharacter

AMachCharacter::AMachCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Set size for collision capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);
	bIsDying = false;

	if (CharacterMovement)
	{
		CharacterMovement->GetNavAgentProperties()->bCanJump = true;
		CharacterMovement->GetNavAgentProperties()->bCanWalk = true;
		CharacterMovement->SetJumpAllowed(true);
		CharacterMovement->SetNetAddressable(); // Make DSO components net addressable
		CharacterMovement->SetIsReplicated(true); // Enable replication by default
	}

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = CapsuleComponent;
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	Mesh->SetOwnerNoSee(true);
}

void AMachCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// TODO: This is just test code
	if (Role == ROLE_Authority)
	{
		Health = BaseHealth;
		Shield = BaseShield;

		FActorSpawnParameters SpawnInfo;
		if (DefaultPrimaryWeapon != NULL)
		{
			PrimaryWeapon = GetWorld()->SpawnActor<AMachWeapon>(DefaultPrimaryWeapon, SpawnInfo);
			SetCurrentWeapon(PrimaryWeapon);
		}

		if (DefaultSecondaryWeapon != NULL)
		{
			SecondaryWeapon = GetWorld()->SpawnActor<AMachWeapon>(DefaultSecondaryWeapon, SpawnInfo);
		}

		if (DefaultTertiaryWeapon != NULL)
		{
			TertiaryWeapon = GetWorld()->SpawnActor<AMachWeapon>(DefaultTertiaryWeapon, SpawnInfo);
		}
	}
}

FName AMachCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

bool AMachCharacter::IsUsePressed()
{
	return bIsUsePressed;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMachCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AMachCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AMachCharacter::OnStopFire);

	InputComponent->BindAxis("MoveForward", this, &AMachCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMachCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AMachCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AMachCharacter::LookUpAtRate);

	InputComponent->BindAction("Equip1", IE_Pressed, this, &AMachCharacter::Equip1);
	InputComponent->BindAction("Equip2", IE_Pressed, this, &AMachCharacter::Equip2);
	InputComponent->BindAction("Equip3", IE_Pressed, this, &AMachCharacter::Equip3);

	InputComponent->BindAction("Use", IE_Pressed, this, &AMachCharacter::OnStartUse);
	InputComponent->BindAction("Use", IE_Released, this, &AMachCharacter::OnStopUse);
}

void AMachCharacter::OnStartFire()
{
	if (!bIsDying && !bFireIntent) {
		bFireIntent = true;
		if (CurrentWeapon != NULL)
		{
			CurrentWeapon->StartFire();
		}
	}
}

void AMachCharacter::OnStopFire()
{
	if (!bIsDying && bFireIntent) {
		bFireIntent = 0;
		if (CurrentWeapon != NULL)
		{
			CurrentWeapon->StopFire();
		}
	}
}

void AMachCharacter::MoveForward(float Value)
{
	if (!bIsDying && Value != 0.0f)
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

void AMachCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMachCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
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
}

void AMachCharacter::OnStopUse()
{
	if (Role < ROLE_Authority)
	{
		ServerOnStopUse();
	}
}

bool AMachCharacter::ServerOnStartUse_Validate()
{
	return true;
}

void AMachCharacter::ServerOnStartUse_Implementation()
{
	bIsUsePressed = true;
}

bool AMachCharacter::ServerOnStopUse_Validate()
{
	return true;
}

void AMachCharacter::ServerOnStopUse_Implementation()
{
	bIsUsePressed = false;
}

void AMachCharacter::OnDeath()
{
	if (!bIsDying)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnDeath"));
		bIsDying = true;

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
	if (Health <= 0.f)
	{
		return false;
	}

	return Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AMachCharacter::ReceiveAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser)
{
	if (Role == ROLE_Authority && Damage > 0.f)
	{
		// Check for friendly fire
		UWorld* World = GetWorld();
		if (World)
		{
			AMachGameMode* GameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode && GameMode->IsFriendlyFireEnabled())
			{
				AMachPlayerState* ThisPlayerState = Cast<AMachPlayerState>(PlayerState);
				AMachPlayerState* OtherPlayerState = Cast<AMachPlayerState>(InstigatedBy->PlayerState);
				if (ThisPlayerState && OtherPlayerState && ThisPlayerState->Team == OtherPlayerState->Team)
				Damage = Damage * GameMode->GetFriendlyFireDamagePercent();
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Dealing %.2f damage"), Damage);

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

		if (Damage > 0.f)
		{
			Health = Health - Damage;
			if (Health <= 0.f)
			{
				Health = 0.f;

				// Set server state, player animation will be handled by health repl
				AMachPlayerState* State = (AMachPlayerState*)PlayerState;
				State->SetIsDead(true);

				Controller->Reset();

				// Despawn player after 5 seconds
				SetLifeSpan(5.0f);
			}
		}
	}
}

void AMachCharacter::RegenerateShield()
{
	if (Role == ROLE_Authority && Shield < BaseShield)
	{
		float RateTime = 0.1f; // How often we're going to tick regen
		float Amount = ShieldRegenRate * RateTime;
		Shield = Shield + Amount;

		GetWorldTimerManager().SetTimer(this, &AMachCharacter::RegenerateShield, RateTime, false);
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

void AMachCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachCharacter, bIsUsePressed);
	DOREPLIFETIME(AMachCharacter, Health);
	DOREPLIFETIME(AMachCharacter, Shield);
	DOREPLIFETIME(AMachCharacter, CurrentWeapon);
	DOREPLIFETIME(AMachCharacter, PrimaryWeapon);
	DOREPLIFETIME(AMachCharacter, SecondaryWeapon);
	DOREPLIFETIME(AMachCharacter, TertiaryWeapon);
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