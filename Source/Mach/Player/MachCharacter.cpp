#include "Mach.h"


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

ETeam::Type AMachCharacter::GetTeam()
{
	AMachPlayerState* State = Cast<AMachPlayerState>(PlayerState);
	if (State)
	{
		return State->Team;
	}

	return ETeam::None;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMachCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	// TODO: MOve all of this to the player controller

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AMachCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AMachCharacter::OnStopFire);

	InputComponent->BindAction("Reload", IE_Pressed, this, &AMachCharacter::OnReload);

	InputComponent->BindAxis("MoveForward", this, &AMachCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMachCharacter::MoveRight);

	InputComponent->BindAction("Equip1", IE_Pressed, this, &AMachCharacter::Equip1);
	InputComponent->BindAction("Equip2", IE_Pressed, this, &AMachCharacter::Equip2);
	InputComponent->BindAction("Equip3", IE_Pressed, this, &AMachCharacter::Equip3);

	InputComponent->BindAction("Use", IE_Pressed, this, &AMachCharacter::OnStartUse);
	InputComponent->BindAction("Use", IE_Released, this, &AMachCharacter::OnStopUse);
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
	if (Role == ROLE_Authority && Damage > 0.f && GetWorld()->GetAuthGameMode())
	{
		AMachGameMode* GameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			Damage = GameMode->DamageAmount(Damage, EventInstigator->GetCharacter(), this);
		}

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

		if (Damage > 0.f)
		{
			Health = Health - Damage;
			if (Health <= 0.f)
			{
				Health = 0.f;
				GameMode->Killed(EventInstigator, Controller, this);
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