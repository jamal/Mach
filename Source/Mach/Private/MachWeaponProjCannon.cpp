// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachWeaponProjCannon.h"
#include "MachGameMode.h"
#include "MachCharacter.h"
#include "MachPlayerState.h"
#include "MachTeamSpawn.h"
#include "DrawDebugHelpers.h"
#include "MachBuildable.h"
#include "MachTurret.h"
#include "MachProjCannonPickerWidget.h"
#include "MachPlayerController.h"
#include "MachVendbot.h"

AMachWeaponProjCannon::AMachWeaponProjCannon(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = Arrow;

	Healing = 5.f;
	Damage = 5.f;
	MaxAmmo = 250.f;
	Ammo = 50.f;
	TimeBetweenShots = 0.075f;
	BurstMode = EWeaponBurstMode::Full;
	Resource = EWeaponResource::Energy;
	Range = 3000.f;
	WeaponModelType = EWeaponModelType::Rifle;
	bActive = false;
	MaxTurrets = 2;
	MaxVendbots = 1;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> BeamParticleObject(TEXT("ParticleSystem'/Game/Particles/P_ProjCannonBeam.P_ProjCannonBeam'"));
	if (BeamParticleObject.Object != NULL)
	{
		BeamParticle = (UParticleSystem*)BeamParticleObject.Object;
	}
}

// TODO: This should probably be a slightly more generic LOS call that can be dome from GameMode or something...
static bool ComponentIsVisibleFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, ECollisionChannel TraceChannel, FHitResult& OutHitResult)
{
	static FName NAME_ComponentIsVisibleFrom = FName(TEXT("ComponentIsVisibleFrom"));
	FCollisionQueryParams LineParams(NAME_ComponentIsVisibleFrom, true, IgnoredActor);

	// Do a trace from origin to middle of box
	UWorld* const World = VictimComp->GetWorld();
	check(World);

	FVector const TraceEnd = VictimComp->Bounds.Origin;
	FVector TraceStart = Origin;
	if (Origin == TraceEnd)
	{
		// tiny nudge so LineTraceSingle doesn't early out with no hits
		TraceStart.Z += 0.01f;
	}
	bool const bHadBlockingHit = World->LineTraceSingle(OutHitResult, TraceStart, TraceEnd, TraceChannel, LineParams);
	//::DrawDebugLine(World, TraceStart, TraceEnd, FLinearColor::Red, true);

	// If there was a blocking hit, it will be the last one
	if (bHadBlockingHit)
	{
		if (OutHitResult.Component == VictimComp)
		{
			// if blocking hit was the victim component, it is visible
			return true;
		}
		else
		{
			// if we hit something else blocking, it's not
			UE_LOG(LogDamage, Log, TEXT("Projection cannon fire to %s blocked by %s (%s)"), *GetNameSafe(VictimComp), *GetNameSafe(OutHitResult.GetActor()), *GetNameSafe(OutHitResult.Component.Get()));
			return false;
		}
	}

	// didn't hit anything, assume nothing blocking the damage and victim is consequently visible
	// but since we don't have a hit result to pass back, construct a simple one, modeling the damage as having hit a point at the component's center.
	FVector const FakeHitLoc = VictimComp->GetComponentLocation();
	FVector const FakeHitNorm = (Origin - FakeHitLoc).SafeNormal();		// normal points back toward the epicenter
	OutHitResult = FHitResult(VictimComp->GetOwner(), VictimComp, FakeHitLoc, FakeHitNorm);
	return true;
}

// TODO: This is a quick impl so that we can do it on the client and server, move it to a better place that isn't AMachGameMode
static bool ActorIsFriendly(const AActor* Causer, const AActor* Receiver)
{
	const AMachCharacter* CauserCharacter = Cast<const AMachCharacter>(Causer);
	if (!CauserCharacter)
	{
		return false;
	}

	AMachPlayerState* CauserPlayerState = Cast<AMachPlayerState>(CauserCharacter->PlayerState);
	if (!CauserPlayerState)
	{
		return false;
	}

	// Damage against a player
	const AMachCharacter* ReceiverCharacter = Cast<const AMachCharacter>(Receiver);
	if (ReceiverCharacter)
	{
		AMachPlayerState* ReceiverPlayerState = Cast<AMachPlayerState>(ReceiverCharacter->PlayerState);
		if (ReceiverPlayerState && CauserPlayerState->Team == ReceiverPlayerState->Team)
		{
			return true;
		}
	}

	// Damage against a spawn
	const AMachTeamSpawn* TeamSpawn = Cast<const AMachTeamSpawn>(Receiver);
	if (TeamSpawn && CauserPlayerState->Team == TeamSpawn->Team)
	{
		return true;
	}

	return false;
}

void AMachWeaponProjCannon::Tick(float DeltaTime)
{
	if (Role < ROLE_Authority)
	{
		// Render beam particles
		if (bActive)
		{
			FVector StartTrace;
			FRotator AimRot;
			GetViewPoint(StartTrace, AimRot);

			const FVector EndTrace = StartTrace + Range * AimRot.Vector();
			const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

			if (Impact.bBlockingHit)
			{
				ParticleComponent->SetBeamTargetPoint(0, Impact.ImpactPoint, 0);
			}
			else
			{
				ParticleComponent->SetBeamTargetPoint(0, EndTrace, 0);
			}

			FVector MuzzleLocation = GetWeaponMesh()->GetSocketLocation(MuzzleAttachPoint);

			// 		for (int i = 0; i < 20; ++i)
			// 		{
			// 			const FVector EndTrace = StartTrace + Range * AimRot.Vector();
			// 			const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
			// 			const FVector Mod = FVector(0.f, i * 5.f - 50.f, 0.f);
			// 
			// 			ParticleComponent->SetBeamSourcePoint(0, MuzzleLocation + Mod, i);
			// 
			// 			if (Impact.bBlockingHit)
			// 			{
			// 				ParticleComponent->SetBeamTargetPoint(0, Impact.ImpactPoint + Mod, i);
			// 			}
			// 			else
			// 			{
			// 				ParticleComponent->SetBeamTargetPoint(0, EndTrace + Mod, i);
			// 			}
			// 		}
		}

		// Managed the current building
		if (CurrentBuild != nullptr)
		{
			FVector Start;
			FRotator Rot;
			GetViewPoint(Start, Rot);
			FVector End = Start + Range * Rot.Vector();

			FHitResult Hit;
			
			FCollisionQueryParams Params(TEXT("ProjCannonBuild"), true, Instigator);
			Params.bTraceAsyncScene = true;

			GetWorld()->LineTraceSingle(Hit, Start, End, ECC_Pawn, Params);
			CurrentBuild->SetPreviewLocation(Hit);
		}
	}
}

void AMachWeaponProjCannon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMachWeaponProjCannon::FireWeapon()
{
	if (!CanFire())
	{
		return;
	}

	ConsumeAmmo();

	FVector Start;
	FRotator AimRot;
	GetViewPoint(Start, AimRot);
	// The box collider is set up from the center, so we want to move forward half the range
	const FVector HalfOrigin = Start + (Range/2.f) * AimRot.Vector();

	static FName NAME_ProjCannonFire = FName(TEXT("ProjCannonFire"));
	FCollisionQueryParams CollisionParams(NAME_ProjCannonFire, false, Instigator);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMulti(Overlaps, HalfOrigin, AimRot.Quaternion(), FCollisionShape::MakeBox(FVector(Range, 40.f, 0.05f)), CollisionParams, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects));

	TMap<AActor*, TArray<FHitResult> > OverlapComponentMap;
	for (int32 Idx = 0; Idx < Overlaps.Num(); ++Idx)
	{
		FOverlapResult const& Overlap = Overlaps[Idx];
		AActor* const OverlapActor = Overlap.GetActor();

		if (OverlapActor &&
			OverlapActor->bCanBeDamaged &&
			OverlapActor != Instigator &&
			Overlap.Component.IsValid())
		{
			FHitResult Hit;
			if (ComponentIsVisibleFrom(Overlap.Component.Get(), Start, Instigator, ECC_Pawn, Hit))
			{
				TArray<FHitResult>& HitList = OverlapComponentMap.FindOrAdd(OverlapActor);
				HitList.Add(Hit);
			}
		}
	}

	for (TMap<AActor*, TArray<FHitResult> >::TIterator It(OverlapComponentMap); It; ++It)
	{
		AActor* const Victim = It.Key();
		TArray<FHitResult> const& ComponentHits = It.Value();

		// TODO: Please fix this
		FPointDamageEvent PointDmg;
		//PointDmg.HitInfo = It.Value();
		PointDmg.ShotDirection = Start;

		if (ActorIsFriendly(Instigator, Victim))
		{
			PointDmg.Damage = Healing * -1;
			float dmg = Victim->TakeDamage(Healing * -1, PointDmg, OwnerPawn->Controller, this);
		}
		else
		{
			PointDmg.Damage = Damage;
			float dmg = Victim->TakeDamage(Damage, PointDmg, OwnerPawn->Controller, this);
		}
	}
}

void AMachWeaponProjCannon::StartFire() {
	// Build will happen on release (StopFire)
	if (CurrentBuild != nullptr)
	{
		// Stop from calling Super::StartFire since CurrentBuild only exists in the client
		return;
	}

	Super::StartFire();

	bActive = true;
	UpdateParticleComponent();
}

void AMachWeaponProjCannon::StopFire() {
	// On Fire release we will try to deploy the current building, this only exists
	// on the client so we will prevent calls to Super::StopFire
	if (Role < ROLE_Authority && CurrentBuild != nullptr && CurrentBuild->CanBeDeployed())
	{
		ServerDeploy(CurrentBuild->BuildableType, CurrentBuild->GetActorLocation(), CurrentBuild->GetActorRotation());

		// Increment local counter
		switch (CurrentBuild->BuildableType)
		{
		case EProjCannonBuildable::Turret:
			NumTurrets = FMath::Clamp<int8>(NumTurrets + 1, 0, MaxTurrets);
			break;
		case EProjCannonBuildable::Vendbot:
			NumVendbots = FMath::Clamp<int8>(NumVendbots + 1, 0, MaxVendbots);
			break;
		}

		CurrentBuild->Destroy();
		CurrentBuild = nullptr;
		return;
	}

	Super::StopFire();

	bActive = false;
	UpdateParticleComponent();
}

void AMachWeaponProjCannon::UpdateParticleComponent()
{
	if (Role < ROLE_Authority)
	{
		if (bActive)
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			if (UseWeaponMesh)
			{
				FRotator Rot;
				if (OwnerPawn != NULL && OwnerPawn->IsFirstPerson())
				{
					Rot = FRotator(9.f, -6.f, 0.f);
				}
				else
				{
					Rot = FRotator::ZeroRotator;
				}
				ParticleComponent = UGameplayStatics::SpawnEmitterAttached(BeamParticle, UseWeaponMesh, MuzzleAttachPoint);
			}
		}
		else if (ParticleComponent)
		{
			ParticleComponent->DestroyComponent();
		}
	}
}

void AMachWeaponProjCannon::StartSecondaryFire()
{
	if (CurrentBuild == nullptr)
	{
		if (PickerWidget != nullptr)
		{
			AMachPlayerController * PC = Cast<AMachPlayerController>(GetInstigatorController());
			if (PC)
			{
				PC->SetInputModeGameAndUI();
			}

			PickerWidget->SetVisibility(ESlateVisibility::Visible);
			PickerWidget->AddToViewport();
		}

	}
	else
	{
		CurrentBuild->Destroy();
		CurrentBuild = nullptr;
	}
}

void AMachWeaponProjCannon::StopSecondaryFire()
{
	if (PickerWidget != nullptr)
	{
		if (PickerWidget->IsVisible())
		{
			// Hide the picker widget
			AMachPlayerController * PC = Cast<AMachPlayerController>(GetInstigatorController());
			if (PC)
			{
				PC->SetInputModeGameOnly();
			}

			PickerWidget->SetVisibility(ESlateVisibility::Hidden);
			PickerWidget->RemoveFromParent();
			
			// Deploy blueprint for selected buildable
			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.Instigator = Instigator;

			switch (PickerWidget->SelectedBuildable)
			{
			case EProjCannonBuildable::Turret:
				if (NumTurrets < MaxTurrets)
				{
					CurrentBuild = Cast<AMachTurret>(GetWorld()->SpawnActor<AMachTurret>(Turret, Params));
				}
				break;

			case EProjCannonBuildable::Vendbot:
				if (NumVendbots < MaxVendbots)
				{
					CurrentBuild = Cast<AMachVendbot>(GetWorld()->SpawnActor<AMachVendbot>(Vendbot, Params));
				}

				break;
			}

			if (CurrentBuild != nullptr)
			{
				CurrentBuild->SetActorEnableCollision(false);
			}
		}
	}
}

void AMachWeaponProjCannon::OnBuildableDestroyed(AMachBuildable* Buildable)
{
	switch (Buildable->BuildableType)
	{
	case EProjCannonBuildable::Turret:
		NumTurrets = FMath::Clamp<int8>(NumTurrets - 1, 0, MaxTurrets);
		break;

	case EProjCannonBuildable::Vendbot:
		NumVendbots = FMath::Clamp<int8>(NumVendbots - 1, 0, NumVendbots);
	}
}

bool AMachWeaponProjCannon::ServerDeploy_Validate(EProjCannonBuildable::Type Buildable, FVector Location, FRotator Rotation)
{
	// TODO: Validate that location is <= range of the buildable
	return true;
}

void AMachWeaponProjCannon::ServerDeploy_Implementation(EProjCannonBuildable::Type Buildable, FVector Location, FRotator Rotation)
{
	AMachBuildable* Build = nullptr;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = Instigator;

	switch (Buildable)
	{
	case EProjCannonBuildable::Turret:
		if (NumTurrets < MaxTurrets)
		{
			Build = GetWorld()->SpawnActor<AMachTurret>(Turret, Location, Rotation, Params);
		}
		break;

	case EProjCannonBuildable::Vendbot:
		if (NumVendbots < MaxVendbots)
		{
			Build = GetWorld()->SpawnActor<AMachVendbot>(Vendbot, Location, Rotation, Params);
		}
		break;

	}

	if (Build != nullptr)
	{
		AMachCharacter* MachCharacter = Cast<AMachCharacter>(GetInstigator());
		if (MachCharacter)
		{
			AMachPlayerState* PS = Cast<AMachPlayerState>(MachCharacter->PlayerState);
			if (PS)
			{
				Build->Team = PS->Team;
			}
		}

		bool bDeployed = Build->Deploy();
		if (bDeployed)
		{
			switch (Buildable)
			{
			case EProjCannonBuildable::Turret:
				NumTurrets = FMath::Clamp<int8>(NumTurrets + 1, 0, MaxTurrets);
				break;

			case EProjCannonBuildable::Vendbot:
				NumVendbots = FMath::Clamp<int8>(NumVendbots + 1, 0, MaxVendbots);
				break;
			}
		}
	}
}

void AMachWeaponProjCannon::OnRep_Active()
{
	UpdateParticleComponent();
}

void AMachWeaponProjCannon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMachWeaponProjCannon, bActive, COND_SkipOwner);
	DOREPLIFETIME(AMachWeaponProjCannon, NumVendbots);
	DOREPLIFETIME(AMachWeaponProjCannon, NumTurrets);
}
