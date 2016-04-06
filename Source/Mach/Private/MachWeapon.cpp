#include "Mach.h"
#include "MachWeapon.h"
#include "MachCharacter.h"
#include "MachTypes.h"
#include "MachImpactEffect.h"
#include "MachProjectile.h"

AMachWeapon::AMachWeapon(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	MaxAmmo = 250;
	AmmoPerClip = 50;
	TimeBetweenShots = 0.1f;
	TimeBetweenSemiBursts = 0.1f;
	BurstMode = EWeaponBurstMode::Full;
	Range = 12000.f;
	BurstCounter = 0;

	bPlayingFireAnim = false;
	bLoopedMuzzleFX = false;
	bIsEquipped = false;
	bEquipPending = false;
	bFireIntent = false;
	bReloadIntent = false;
	bRefiring = false;

	// Create an arrow component to use as the root
	Arrow = PCIP.CreateDefaultSubobject<UArrowComponent>(this, TEXT("ArrowComponent"));
	Arrow->SetHiddenInGame(true);

	// Create a mesh component that will be used when being viewed from a '1st person' view
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	Mesh3P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	Mesh3P->SetOwnerNoSee(true);
}

void AMachWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TotalAmmo = MaxAmmo;
	Ammo = AmmoPerClip;
}

void AMachWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Role == ROLE_Authority)
	{
		if (Resource == EWeaponResource::Energy || Resource == EWeaponResource::EnergyLimited)
		{
			if (CurrentState == EWeaponState::Firing)
			{
				float Delta = EnergyRate * DeltaSeconds;

				Ammo -= Delta;
				if (Ammo <= 0.f) {
					UpdateWeaponState();
				}
			}
			else if (Ammo < AmmoPerClip)
			{
				float Delta = EnergyRechargeRate * DeltaSeconds;

				if (Resource == EWeaponResource::EnergyLimited)
				{
					if (TotalAmmo < Delta)
					{
						Delta = TotalAmmo;
					}

					TotalAmmo -= Delta;
				}


				Ammo += Delta;
				if (Ammo > AmmoPerClip)
				{
					Ammo = AmmoPerClip;
				}
			}
		}
	}
}

void AMachWeapon::SetOwningPawn(AMachCharacter* NewPawn)
{
	// Instigator is an AActor property to determine damage owner.
	Instigator = NewPawn;
	OwnerPawn = NewPawn;
	SetOwner(NewPawn);
}

void AMachWeapon::OnEquip()
{
	if (!bIsEquipped)
	{
		// TODO: Play equip animation
		AttachMesh();

		bEquipPending = true;
		UpdateWeaponState();

		// Wait for equip animation to finish
		float Duration = .5f;
		GetWorldTimerManager().SetTimer(this, &AMachWeapon::OnEquipFinished, Duration, false);
	}
}

void AMachWeapon::OnUnequip()
{
	DetachMesh();

	if (bIsEquipped)
	{
		bIsEquipped = false;
		bReloadIntent = false;
		bFireIntent = false;

		UpdateWeaponState();
	}
}

void AMachWeapon::AttachMesh()
{
	if (OwnerPawn)
	{
		Mesh1P->SetHiddenInGame(false);
		Mesh1P->AttachTo(OwnerPawn->Mesh1P, OwnerPawn->GetWeaponAttachPoint());

		Mesh3P->SetHiddenInGame(false);
		Mesh3P->AttachTo(OwnerPawn->Mesh, OwnerPawn->GetWeaponAttachPoint());
	}
}

void AMachWeapon::DetachMesh()
{
	Mesh1P->DetachFromParent();
	Mesh1P->SetHiddenInGame(true);
	
	Mesh3P->DetachFromParent();
	Mesh3P->SetHiddenInGame(true);
}

void AMachWeapon::OnEquipFinished()
{
	bIsEquipped = true;
	UpdateWeaponState();
}

void AMachWeapon::StartFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bFireIntent)
	{
		bFireIntent = true;
		UpdateWeaponState();
	}
}

bool AMachWeapon::ServerStartFire_Validate()
{
	return true;
}

void AMachWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

void AMachWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bFireIntent)
	{
		bFireIntent = false;
		UpdateWeaponState();
	}
}

bool AMachWeapon::ServerStopFire_Validate()
{
	return true;
}

void AMachWeapon::ServerStopFire_Implementation()
{
	StopFire();
}
void AMachWeapon::StartSecondaryFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStartSecondaryFire();
	}
}

bool AMachWeapon::ServerStartSecondaryFire_Validate()
{
	return true;
}

void AMachWeapon::ServerStartSecondaryFire_Implementation()
{
	StartSecondaryFire();
}

void AMachWeapon::StopSecondaryFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopSecondaryFire();
	}
}

bool AMachWeapon::ServerStopSecondaryFire_Validate()
{
	return true;
}

void AMachWeapon::ServerStopSecondaryFire_Implementation()
{
	StopSecondaryFire();
}

void AMachWeapon::Reload()
{
	if (Role < ROLE_Authority)
	{
		ServerReload();
	}

	if (!bReloadIntent && CurrentState != EWeaponState::Reloading)
	{
		bReloadIntent = true;
		UpdateWeaponState();
	}
}

bool AMachWeapon::ServerReload_Validate()
{
	return true;
}

void AMachWeapon::ServerReload_Implementation()
{
	Reload();
}

bool AMachWeapon::CanReload()
{
	return Resource == EWeaponResource::Ammo && TotalAmmo > 0 && Ammo < AmmoPerClip;
}

bool AMachWeapon::CanFire()
{
	return Ammo > 0;
}

void AMachWeapon::UpdateWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		// We will start reloading when the user presses reload, or if they try to fire with no ammo
		if (bFireIntent && !CanFire()) {
			bReloadIntent = true;
		}

		if (bReloadIntent && CanReload())
		{
			NewState = EWeaponState::Reloading;
		}
		else if (bFireIntent && CanFire())
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bEquipPending)
	{
		NewState = EWeaponState::Equipping;
	}

	SetCurrentState(NewState);
}

void AMachWeapon::SetCurrentState(EWeaponState::Type NewState)
{
	const EWeaponState::Type PrevState = CurrentState;
	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}

	if (PrevState != EWeaponState::Reloading && NewState == EWeaponState::Reloading)
	{
		OnReloadStarted();
	}
}

void AMachWeapon::OnBurstStarted()
{
	const float fCurrentTime = GetWorld()->GetTimeSeconds();
	fLastBurstTime = fCurrentTime;

	if (fLastFireTime > 0.f && fCurrentTime < (fLastFireTime + TimeBetweenShots))
	{
		GetWorldTimerManager().SetTimer(this, &AMachWeapon::HandleFiring, TimeBetweenShots - (fCurrentTime - fLastBurstTime), false);
	}
	else
	{
		HandleFiring();
	}
}

void AMachWeapon::OnBurstFinished()
{
	// Stop firing FX on remote clients
	if (Role == ROLE_Authority)
	{
		BurstCounter = 0;
	}

	if (BurstMode == EWeaponBurstMode::Charge)
	{
		FireWeapon();
	}

	if (bPlayingFireAnim)
	{
		StopWeaponAnimation(FireAnim);
		bPlayingFireAnim = false;
	}

	GetWorldTimerManager().ClearTimer(this, &AMachWeapon::HandleFiring);
	bRefiring = false;
}

void AMachWeapon::OnReloadStarted()
{
	// TODO: Actually fix this, tie to animation instead of a timer
	UE_LOG(LogTemp, Warning, TEXT("reload started"));
	GetWorldTimerManager().SetTimer(this, &AMachWeapon::OnReloadFinished, 2, false);
}

void AMachWeapon::OnReloadFinished()
{
	bReloadIntent = false;

	int32 Refill = AmmoPerClip - Ammo;
	TotalAmmo -= Refill;
	if (TotalAmmo < 0)
	{
		TotalAmmo = 0;
		Refill += TotalAmmo;
	}

	Ammo += Refill;

	UE_LOG(LogTemp, Warning, TEXT("reload finished, Refill: %d, Ammo: %d, Total Ammo: %d"), Refill, Ammo, TotalAmmo);

	UpdateWeaponState();
}

void AMachWeapon::HandleFiring()
{
	if (BurstMode != EWeaponBurstMode::Charge)
	{
		FireWeapon();
	}

	// Determine if we're refiring based on burst mode
	bRefiring = (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f);
	if (bRefiring && BurstMode == EWeaponBurstMode::Semi)
	{
		const float fCurrentTime = GetWorld()->GetTimeSeconds();
		if (fLastBurstTime < (fCurrentTime - TimeBetweenSemiBursts))
		{
			bRefiring = false;
		}
	}
	else if ((bRefiring && BurstMode == EWeaponBurstMode::Single) || BurstMode == EWeaponBurstMode::Charge)
	{
		bRefiring = false;
	}

	if (bRefiring && CanFire())
	{
		GetWorldTimerManager().SetTimer(this, &AMachWeapon::HandleFiring, TimeBetweenShots, false);
	}

	if (!CanFire())
	{
		OnBurstFinished();
	}

	fLastFireTime = GetWorld()->GetTimeSeconds();
}

FVector AMachWeapon::GetViewRotation()
{
	AController* controller = Instigator ? Instigator->Controller : NULL;
	FRotator rotation;

	if (controller)
	{
		FVector _;
		controller->GetPlayerViewPoint(_, rotation);
	}
	else if (Instigator)
	{
		rotation = Instigator->GetViewRotation();
	}

	return rotation.Vector();
}

void AMachWeapon::GetViewPoint(FVector& start, FRotator& rotation)
{
	AController* controller = Instigator ? Instigator->Controller : NULL;

	if (controller)
	{
		controller->GetPlayerViewPoint(start, rotation);
	}
	else if (Instigator)
	{
		rotation = Instigator->GetViewRotation();
	}
}

void AMachWeapon::SpawnImpactEffects(const FHitResult& impact)
{
	// Dirty hack to have two surface types for shield and flesh
	AMachCharacter* Character = Cast<AMachCharacter>(impact.Actor.Get());
	if (Character != NULL)
	{
		UPhysicalMaterial* PhysMaterial = impact.PhysMaterial.Get();
		if (PhysMaterial != NULL)
		{
			if (Character->Shield > 0.f)
			{
				PhysMaterial->SurfaceType = MACH_SURFACE_Shield;
			}
			else
			{
				PhysMaterial->SurfaceType = MACH_SURFACE_Flesh;
			}
		}
	}

	AMachImpactEffect* effect = GetWorld()->SpawnActorDeferred<AMachImpactEffect>(ImpactTemplate, impact.ImpactPoint, impact.ImpactNormal.Rotation());
	if (effect)
	{
		effect->SurfaceHit = impact;
		UGameplayStatics::FinishSpawningActor(effect, FTransform(impact.ImpactNormal.Rotation(), impact.ImpactPoint));
	}
}

void AMachWeapon::SpawnTrailEffect(const FVector& EndPoint)
{
	return;
	if (TrailFX)
	{
		const FVector Origin = GetMuzzleLocation();

		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin);
		if (TrailPSC)
		{
			TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
		}
	}
}

float AMachWeapon::PlayWeaponAnimation(const FWeaponAnim& Animation)
{
	float Duration = 0.0f;
	/*
	if (OwnerPawn)
	{
		UAnimMontage* UseAnim = OwnerPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			Duration = OwnerPawn->PlayAnimMontage(UseAnim);
		}
	}
	*/
	
	return Duration;
}

void AMachWeapon::StopWeaponAnimation(const FWeaponAnim& Animation)
{
	/*
	if (OwnerPawn)
	{
		UAnimMontage* UseAnim = OwnerPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			OwnerPawn->StopAnimMontage(UseAnim);
		}
	}
	*/
}

void AMachWeapon::ConsumeAmmo()
{
	if (Resource == EWeaponResource::Ammo)
	{
		Ammo -= 1;
	}
}

void AMachWeapon::FireWeapon()
{
	if (!CanFire())
	{
		return;
	}

	ConsumeAmmo();

	if (ProjectileClass != NULL) {
		if (Role < ROLE_Authority && OwnerPawn->IsLocallyControlled())
		{
			FireProjectile();
		}

		MulticastFireProjectile();
	}
	else
	{
		FVector start;
		FRotator rotation;
		GetViewPoint(start, rotation);
		const FVector end = start + Range * rotation.Vector();

		const FHitResult impact = WeaponTrace(start, end);
		ProcessHit(impact, start, end);
	}

	if (!bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnim);
		bPlayingFireAnim = true;
	}

	/*

		// TODO: This should be determined by roles
		if (Role == ROLE_Authority) {
			HitImpact = impact;
			NotifyHit(impact, start);
		}

		// Play impact for the owner if this is a listen server
		if (GetNetMode() != NM_DedicatedServer)
		{
			SpawnImpactEffects(impact);
		}
	}
	else
	{
		//ServerNotifyMiss(start);
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwnerPawn->GetActorLocation());
	}

	// TODO: Need to move weapon mesh out of character blueprint
	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
	// Get the animation object for the arms mesh
	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	if (AnimInstance != NULL)
	{
	AnimInstance->Montage_Play(FireAnimation, 1.f);
	}
	}
	*/
}

void AMachWeapon::FireProjectile()
{
	FVector AimLoc;
	FRotator AimRot;
	GetViewPoint(AimLoc, AimRot);

	FVector Muzzle = GetMuzzleLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Instigator;
	SpawnParams.Instigator = Instigator;

	AMachProjectile* Projectile = Cast<AMachProjectile>(GetWorld()->SpawnActor<AMachProjectile>(ProjectileClass, Muzzle, AimRot, SpawnParams));
	if (Projectile)
	{
		Projectile->Weapon = this;
		Projectile->Damage = Damage;
		Projectile->DamageRadius = DamageRadius;
		Projectile->DamageType = UDamageType::StaticClass();
	}
	// Replicate weapon firing
	BurstCounter++;

	SimulateWeaponFiring();
}

USkeletalMeshComponent* AMachWeapon::GetWeaponMesh() const
{
	return (OwnerPawn != NULL && OwnerPawn->IsFirstPerson()) ? Mesh1P : Mesh3P;
}

FVector AMachWeapon::GetMuzzleLocation() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketLocation(MuzzleAttachPoint);
}

FVector AMachWeapon::GetMuzzleDirection() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}

void AMachWeapon::ProcessHit(const FHitResult& Impact, const FVector& StartTrace, const FVector& EndTrace)
{
	ProcessHit(Damage, Impact, StartTrace, EndTrace);
}

void AMachWeapon::ProcessHit(const float DealingDamage, const FHitResult& Impact, const FVector& StartTrace, const FVector& EndTrace)
{
	// Replicate the impact effect
	if (Role == ROLE_Authority)
	{
		HitImpact = Impact;
		BurstCounter++;
	}

	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

	// Deal damage
	if (Impact.GetActor())
	{
		// Spawn effects on the client
		SpawnImpactEffects(Impact);

		FPointDamageEvent PointDmg;
		PointDmg.HitInfo = Impact;
		PointDmg.ShotDirection = StartTrace;
		PointDmg.Damage = DealingDamage;

		if (Role == ROLE_Authority)
		{

		}
		UE_LOG(LogTemp, Warning, TEXT("Process HIT for %.2f"), DealingDamage);

		float dmg = Impact.GetActor()->TakeDamage(DealingDamage, PointDmg, OwnerPawn->Controller, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Miss"), DealingDamage);
		// Hit a non-actor surface
		if (Impact.bBlockingHit)
		{
			SpawnImpactEffects(Impact);
		}
	}

	//SpawnTrailEffect(EndTrace);
	SimulateWeaponFiring();
}

void AMachWeapon::SimulateWeaponFiring()
{
	if (MuzzleFX)
	{
		USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
		if (!bLoopedMuzzleFX || MuzzlePSC == NULL)
		{
			MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, UseWeaponMesh, MuzzleAttachPoint);
		}
	}
}

FHitResult AMachWeapon::WeaponTrace(const FVector& start, const FVector& end) const
{
	const FName TraceTag("WeaponTrace");

	FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult impact(ForceInit);
	GetWorld()->LineTraceSingle(impact, start, end, ECC_Pawn, TraceParams);
	return impact;
}

void AMachWeapon::NotifyHit(const FHitResult& impact, const FVector& origin)
{
	//ServerNotifyHit(impact, origin);
}

bool AMachWeapon::ServerNotifyHit_Validate(const FHitResult impact, const FVector origin)
{
	// TODO: Validate on the server-side that the hit was possible
	return true;
}

void AMachWeapon::ServerNotifyHit_Implementation(const FHitResult impact, const FVector origin)
{
}

void AMachWeapon::MulticastFireProjectile_Implementation()
{
	if (OwnerPawn != NULL && !OwnerPawn->IsLocallyControlled())
	{
		FireProjectile();
	}
}

//////////////////////////////////////////////////////////////////////////
// Replication

void AMachWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMachWeapon, HitImpact, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMachWeapon, BurstCounter, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AMachWeapon, Ammo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AMachWeapon, TotalAmmo, COND_OwnerOnly);
}

void AMachWeapon::OnRep_BurstCounter()
{
	SpawnImpactEffects(HitImpact);
	SimulateWeaponFiring();
}