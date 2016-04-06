// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Mach.h"
#include "MachGameMode.h"
#include "MachPlayerState.h"
#include "MachPlayerStart.h"
#include "MachWeapon.h"
#include "MachPlayerController.h"
#include "MachTeamSpawn.h"
#include "MachCharacter.h"
#include "MachTechTree.h"
#include "MachHUD.h"

AMachGameMode::AMachGameMode(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerControllerObject(TEXT("Blueprint'/Game/Blueprints/PlayerController.PlayerController'"));
	if (PlayerControllerObject.Object != NULL)
	{
		PlayerControllerClass = (UClass*)PlayerControllerObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> HeavyArmorObject(TEXT("Blueprint'/Game/Blueprints/ArmorTypes/HeavyArmor.HeavyArmor'"));
	if (HeavyArmorObject.Object != NULL)
	{
		HeavyArmorPawnClass = (UClass*)HeavyArmorObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> MediumArmorObject(TEXT("Blueprint'/Game/Blueprints/ArmorTypes/MediumArmor.MediumArmor'"));
	if (MediumArmorObject.Object != NULL)
	{
		MediumArmorPawnClass = (UClass*)MediumArmorObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> LightArmorObject(TEXT("Blueprint'/Game/Blueprints/ArmorTypes/LightArmor.LightArmor'"));
	if (LightArmorObject.Object != NULL)
	{
		LightArmorPawnClass = (UClass*)LightArmorObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> StealthArmorObject(TEXT("Blueprint'/Game/Blueprints/ArmorTypes/StealthArmor.StealthArmor'"));
	if (StealthArmorObject.Object != NULL)
	{
		StealthArmorPawnClass = (UClass*)StealthArmorObject.Object->GeneratedClass;
	}

	static ConstructorHelpers::FClassFinder<AHUD> HudObject(TEXT("/Game/Blueprints/MachHUD.MachHUD_C"));
	if (HudObject.Class != NULL)
	{
		HUDClass = HudObject.Class;
	}

	PlayerStateClass = AMachPlayerState::StaticClass();
	GameStateClass = AMachGameState::StaticClass();

	bDelayedStart = false;
	bFriendlyFire = true;
	FriendlyFireDamagePercent = .5;

	bRestartPostMatch = true;
	PregameTime = 0.f;
	PostMatchTime = 30.f;

	RespawnWaveDeathLimit = 5.f;
	RespawnWaveTime = 15.f;
	
	SetTickableWhenPaused(true);
}

void AMachGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game listening on %s"), *GetWorld()->GetNetDriver()->LowLevelGetNetworkNumber());
	}

	TechTreeTeamA = GetWorld()->SpawnActor<AMachTechTree>();
	TechTreeTeamA->Team = ETeam::A;

	TechTreeTeamB = GetWorld()->SpawnActor<AMachTechTree>();
	TechTreeTeamB->Team = ETeam::B;

	CurrentRespawnTimer = RespawnWaveTime;

	GetWorldTimerManager().SetTimer(this, &AMachGameMode::UpdatePlayerVisibility, 0.5f, true);
}

UClass* AMachGameMode::GetDefaultPawnClassForController(AController* InController)
{
	AMachPlayerController* MachController = Cast<AMachPlayerController>(InController);
	if (MachController)
	{
		switch (MachController->PlayerPawnData.Armor)
		{
		case EPlayerArmor::Light:
			UE_LOG(LogTemp, Warning, TEXT("Using light armor pawn"));
			return LightArmorPawnClass;

		case EPlayerArmor::Stealth:
			UE_LOG(LogTemp, Warning, TEXT("Using stealth armor pawn"));
			return StealthArmorPawnClass;

		case EPlayerArmor::Medium:
			UE_LOG(LogTemp, Warning, TEXT("Using medium armor pawn"));
			return MediumArmorPawnClass;

		case EPlayerArmor::Heavy:
		default:
			UE_LOG(LogTemp, Warning, TEXT("Using heavy armor pawn"));
			return HeavyArmorPawnClass;
		}
	}

	return DefaultPawnClass;
}

void AMachGameMode::SetFriendlyFire(bool bEnabled)
{
	bFriendlyFire = bEnabled;
}

bool AMachGameMode::IsFriendlyFireEnabled()
{
	return bFriendlyFire;
}

float AMachGameMode::GetFriendlyFireDamagePercent()
{
	return FriendlyFireDamagePercent;
}

bool AMachGameMode::DamageIsFriendly(const AActor* Causer, const AActor* Receiver) const
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

bool AMachGameMode::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, const AActor* Causer, const AActor* Receiver) const
{
	if (Damage == 0.f || !Receiver->bCanBeDamaged || !GameState->IsMatchInProgress())
	{
		return false;
	}

	const AMachCharacter* ReceiverCharacter = Cast<const AMachCharacter>(Receiver);
	if (ReceiverCharacter && ReceiverCharacter->Health == 0.f)
	{
		return false;
	}

	if (!bFriendlyFire && DamageIsFriendly(Causer, Receiver))
	{
		return false;
	}

	return true;
}

float AMachGameMode::ShieldMultiplierPower(int32 Power) const
{
	return .1f * FMath::Clamp(Power, 0, 4) + 1.f;
}

float AMachGameMode::DamageMultiplierPower(int32 Power) const
{
	return .05f * FMath::Clamp(Power, 0, 4) + 1.f;
}

float AMachGameMode::ShieldMultiplier(const AActor* Player) const
{
	const AMachCharacter* PlayerCharacter = Cast<const AMachCharacter>(Player);
	if (!PlayerCharacter)
	{
		return 1.f;
	}

	AMachPlayerState* PlayerState = Cast<AMachPlayerState>(PlayerCharacter->PlayerState);
	if (!PlayerState)
	{
		return 1.f;
	}

	return ShieldMultiplierPower(PlayerState->TechTree->Power);
}

float AMachGameMode::DamageMultiplier(const AActor* Causer) const
{
	const AMachCharacter* CauserCharacter = Cast<const AMachCharacter>(Causer);
	if (!CauserCharacter)
	{
		return 1.f;
	}

	AMachPlayerState* PlayerState = Cast<AMachPlayerState>(CauserCharacter->PlayerState);
	if (!PlayerState)
	{
		return 1.f;
	}

	UE_LOG(LogTemp, Warning, TEXT("Damage bonus %.2f"), CauserCharacter->DamageBonusMultiplier);
	
	return DamageMultiplierPower(PlayerState->TechTree->Power) + CauserCharacter->DamageBonusMultiplier;
}

float AMachGameMode::DamageAmount(float Damage, const AActor* Causer, const AActor* Receiver) const
{
	if (Damage > 0.f && bFriendlyFire && FriendlyFireDamagePercent < 1.f && DamageIsFriendly(Causer, Receiver))
	{
		return Damage * FriendlyFireDamagePercent * DamageMultiplier(Causer);
	}

	return Damage * DamageMultiplier(Causer);
}

void AMachGameMode::UncaptureTechPoint(ETeam::Type OldTeam)
{
	switch (OldTeam)
	{
	case ETeam::A:
		TechTreeTeamA->Power -= 1;
		break;
	case ETeam::B:
		TechTreeTeamB->Power -= 1;
		break;
	}
}

void AMachGameMode::CaptureTechPoint(ETeam::Type Team)
{
	switch (Team)
	{
	case ETeam::A:
		TechTreeTeamA->Power += 1;
		break;
	case ETeam::B:
		TechTreeTeamB->Power += 1;
		break;
	}
}

void AMachGameMode::UpdateTeamPower(ETeam::Type Team, int32 Power)
{
	return;
	UE_LOG(LogTemp, Warning, TEXT("Updateteampower %d"), Power);
	
	AMachGameState* State = Cast<AMachGameState>(GameState);
	if (State)
	{
		auto Players = State->GetPlayers(Team);
		for (auto Player : Players)
		{
			// Update shield to new power
			AController* Controller = Cast<AController>(Player->GetOwner());
			AMachCharacter* Character = Cast<AMachCharacter>(Controller->GetPawn());
			if (Character)
			{
				UE_LOG(LogTemp, Warning, TEXT("Updating shield value"));
				Character->SetShieldMultiplier(ShieldMultiplierPower(Power));
			}
		}
	}
}

void AMachGameMode::GameOver(ETeam::Type WinningTeam)
{
	UE_LOG(LogTemp, Warning, TEXT("Game is now over"));
	if (WinningTeam == ETeam::A)
	{
		UE_LOG(LogTemp, Warning, TEXT("Team A has won"));
	}
	else if (WinningTeam == ETeam::B)
	{
		UE_LOG(LogTemp, Warning, TEXT("Team B has won"));
	}

	EndMatch();
	AMachGameState* State = Cast<AMachGameState>(GameState);
	if (State)
	{
		State->WinningTeam = WinningTeam;
	}

	if (bRestartPostMatch)
	{
		GetWorldTimerManager().SetTimer(this, &AMachGameMode::RestartGame, PostMatchTime, true);
	}
}

void AMachGameMode::Killed(AController* Killer, AController* KilledPlayer, AMachCharacter* KilledCharacter)
{
	AMachPlayerState* KillerState = Cast<AMachPlayerState>(Killer->PlayerState);
	AMachPlayerState* KilledPlayerState = Cast<AMachPlayerState>(KilledPlayer->PlayerState);

	if (KillerState && KilledPlayerState)
	{
		KilledCharacter->Health = 0.f;
		KilledCharacter->SetLifeSpan(5.0f); // Despawn player after 5 seconds

		KilledPlayerState->SetIsDead(true);
		// Remove player controller from dead pawn
		KilledPlayer->Reset();

		// Record kill
		if (KillerState->Team != KilledPlayerState->Team)
		{
			KillerState->ScoreKill(KilledPlayerState);
			
			// Increase kill streak
			AMachCharacter* Character = Cast<AMachCharacter>(Killer->GetPawn());
			if (Character)
			{
				Character->Killstreak++;
				UE_LOG(LogTemp, Warning, TEXT("Increased killstreak %d"), Character->Killstreak);
			}
		}

		// Record assists
		for (AMachPlayerState* AssistPlayer : KilledPlayerState->AssistPlayers)
		{
			if (AssistPlayer->PlayerId != KillerState->PlayerId)
			{
				AssistPlayer->ScoreAssist(KilledPlayerState);
			}
		}

		// Record death
		KilledPlayerState->ScoreDeath(KillerState);

		// Reset kill streak
		AMachCharacter* Character = Cast<AMachCharacter>(KilledPlayer->GetPawn());
		if (Character)
		{
			Character->Killstreak++;
			UE_LOG(LogTemp, Warning, TEXT("Reset killstreak %d"), Character->Killstreak);
		}

		// Add player to the next spawn wave if it's not too close
		if (CurrentRespawnTimer > RespawnWaveDeathLimit)
		{
			RespawnWavePlayers.Add(KilledPlayer);
		}
		else
		{
			NextRespawnWavePlayers.Add(KilledPlayer);
		}

		// TODO: Broadcast kill to all clients to display in HUD
	}
}

FString AMachGameMode::InitNewPlayer(APlayerController* NewPlayerController, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal)
{
	check(NewPlayerController);

	FString ErrorMessage;

	UE_LOG(LogTemp, Warning, TEXT("InitNewPlayer"));

	EPlayerArmor::Type Armor;
	EPlayerPrimaryWeapon::Type Weapon;

	FString ArmorOption = ParseOption(Options, "Armor");
	if (!ArmorOption.IsEmpty())
	{
		Armor = EPlayerArmor::FromInt(FCString::Atoi(*ArmorOption));
	}

	if (ArmorOption.IsEmpty() || Armor == EPlayerArmor::None)
	{
		Armor = EPlayerArmor::Light;
	}

	FString WeaponOption = ParseOption(Options, "Weapon");
	if (!WeaponOption.IsEmpty())
	{
		Weapon = EPlayerPrimaryWeapon::FromInt(FCString::Atoi(*WeaponOption));
	}

	if (WeaponOption.IsEmpty() || Weapon == EPlayerPrimaryWeapon::None)
	{
		Weapon = EPlayerPrimaryWeapon::Rifle;
	}

	AMachPlayerController* MachController = Cast<AMachPlayerController>(NewPlayerController);
	if (MachController)
	{
		MachController->PlayerPawnData.Armor = Armor;
		MachController->PlayerPawnData.PrimaryWeapon = Weapon;
	}

	// Register the player with the session
	GameSession->RegisterPlayer(NewPlayerController, UniqueId, HasOption(Options, TEXT("bIsFromInvite")));

	// Init player's name
	FString InName = ParseOption(Options, TEXT("Name")).Left(20);
	if (InName.IsEmpty())
	{
		InName = FString::Printf(TEXT("%s%i"), *DefaultPlayerName, NewPlayerController->PlayerState->PlayerId);
	}

	ChangeName(NewPlayerController, InName, false);

	// TODO: Login should talk to the world server to determine what team this player is assigned to
	// Dummy logic to assign players to teams
	AMachPlayerState* PlayerState = (AMachPlayerState*)NewPlayerController->PlayerState;
	if ((NumPlayersTeamA + NumPlayersTeamB) % 2 == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding player to team A"));
		PlayerState->SetTeam(ETeam::A);
		PlayerState->TechTree = TechTreeTeamA;
		NumPlayersTeamA++;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding player to team B"));
		PlayerState->SetTeam(ETeam::B);
		PlayerState->TechTree = TechTreeTeamB;
		NumPlayersTeamB++;
	}

	// Find a starting spot
	AActor* const StartSpot = FindPlayerStart(NewPlayerController, Portal);
	if (StartSpot != NULL)
	{
		// Set the player controller / camera in this new location
		FRotator InitialControllerRot = StartSpot->GetActorRotation();
		InitialControllerRot.Roll = 0.f;
		NewPlayerController->SetInitialLocationAndRotation(StartSpot->GetActorLocation(), InitialControllerRot);
		NewPlayerController->StartSpot = StartSpot;
	}
	else
	{
		ErrorMessage = FString::Printf(TEXT("Failed to find PlayerStart"));
	}

	return ErrorMessage;
}

AActor* AMachGameMode::ChoosePlayerStart(AController* Player)
{
	ETeam::Type PlayerTeam = ((AMachPlayerState*)Player->PlayerState)->GetTeam();

	for (APlayerStart* BasePlayerStart : PlayerStarts)
	{
		AMachPlayerStart* PlayerStart = (AMachPlayerStart*)BasePlayerStart;
		if (PlayerStart->Team == PlayerTeam)
		{
			return PlayerStart;
		}
	}

	return NULL;
}

bool AMachGameMode::IsMatchInProgress() const
{
	if (GetMatchState() == MatchState::PreGame)
	{
		return true;
	}

	return Super::IsMatchInProgress();
}


void AMachGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	UE_LOG(LogTemp, Warning, TEXT("Match is starting"));
}

void AMachGameMode::RespawnPlayers()
{
	UE_LOG(LogTemp, Warning, TEXT("Respawn players (current wave: %.2f, next wave: %.2f)"), RespawnWavePlayers.Num(), NextRespawnWavePlayers.Num());

	auto ThisRespawn(RespawnWavePlayers);
	for (AController* Controller : ThisRespawn)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (PlayerController)
		{
			AMachPlayerState* Player = Cast<AMachPlayerState>(PlayerController->PlayerState);
			if (Player->IsDead())
			{
				Player->SetIsDead(false);
				RestartPlayer(Controller);

				if (Controller->GetPawn() != NULL)
				{
					Controller->GetPawn()->PlayerState = Controller->PlayerState;
				}
			}
		}
	}

	RespawnWavePlayers = MoveTemp(NextRespawnWavePlayers);
}

void AMachGameMode::Tick(float DeltaSeconds)
{
	if (GetMatchState() == MatchState::WaitingToStart || GetMatchState() == MatchState::PreGame)
	{
		// We won't start counting down until we have more than 1 player
		if (GetMatchState() == MatchState::PreGame && NumPlayers + NumBots > 1)
		{
			PregameStartTime += DeltaSeconds;
		}
		else if (GetMatchState() == MatchState::PreGame)
		{
			PregameStartTime = 0.f;
		}

		// Check to see if we should start the match
		if (ReadyToStartMatch())
		{
			StartMatch();
		}
	}

	if (GetMatchState() == MatchState::WaitingToStart)
	{
		if (ReadyToStartPreGame())
		{
			StartPreGame();
		}
	}

	// Respawn timer
	if (GetMatchState() == MatchState::InProgress)
	{
		CurrentRespawnTimer -= DeltaSeconds;
		if (CurrentRespawnTimer <= 0.f)
		{
			CurrentRespawnTimer = RespawnWaveTime;
			RespawnPlayers();
		}
	}

	Super::Tick(DeltaSeconds);
}

bool AMachGameMode::PlayerCanRestart(APlayerController* Player)
{
	// Allow player to spawn in pre-game
	if (GetMatchState() == MatchState::PreGame)
	{
		return Player->CanRestartPlayer();
	}

	return Super::PlayerCanRestart(Player);
}

bool AMachGameMode::ReadyToStartMatch()
{
	// Assuming bDelayedStart because we don't care
	// Only start if we have more than 1 player
	//if ((GetMatchState() == MatchState::WaitingToStart || (GetMatchState() == MatchState::PreGame && PregameStartTime >= PregameTime)) && NumPlayers + NumBots > 1)
	if (GetMatchState() == MatchState::WaitingToStart || GetMatchState() == MatchState::PreGame)
	{
		return true;
	}

	return false;
}

bool AMachGameMode::ReadyToStartPreGame()
{
	if (GetMatchState() == MatchState::WaitingToStart && NumPlayers > 0)
	{
		return true;
	}

	return false;
}

void AMachGameMode::StartPreGame()
{
	if (GetMatchState() != MatchState::WaitingToStart)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Setting match to pre-game"));

	SetMatchState(MatchState::PreGame);
	HandleMatchHasStarted();
	PregameStartTime = 0.f;
}

void AMachGameMode::SwapPawn(AMachPlayerController* Controller)
{
	if (AllowCheats(Controller))
	{
		APawn* OldPawn = Controller->GetPawn();

		FRotator StartRotation(ForceInit);
		StartRotation.Yaw = OldPawn->GetActorRotation().Yaw;
		FVector StartLocation = OldPawn->GetActorLocation();

		GetWorld()->DestroyActor(OldPawn);

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = Instigator;
		APawn* NewPawn = GetWorld()->SpawnActor<APawn>(GetDefaultPawnClassForController(Controller), StartLocation, StartRotation, SpawnInfo);

		Controller->SetPawn(NewPawn);

		if (Controller->GetPawn() == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn new pawn"));

			Controller->FailedToSpawnPawn();
			return;
		}

		Controller->Possess(Controller->GetPawn());

		if (Controller->GetPawn() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to posses new pawn"));

			Controller->FailedToSpawnPawn();
			return;
		}
		
		// set initial control rotation to player start's rotation
		Controller->ClientSetRotation(Controller->GetPawn()->GetActorRotation(), true);

		FRotator NewControllerRot = OldPawn->GetActorRotation();
		NewControllerRot.Roll = 0.f;
		Controller->SetControlRotation(NewControllerRot);

		SetPlayerDefaults(Controller->GetPawn());
	}
}

void AMachGameMode::StartNewPlayer(APlayerController* NewPlayer)
{
	Super::StartNewPlayer(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("PostLogin"));

	AMachHUD* MachHUD = Cast<AMachHUD>(NewPlayer->GetHUD());
	if (MachHUD)
	{
		FString PlayerJoin = FText::Format( NSLOCTEXT("Login", "PlayerJoined", "{0} has joined the game."), FText::FromString(NewPlayer->PlayerState->GetName())).ToString();
		Broadcast(NewPlayer->GetPawn(), PlayerJoin);
	}
}

void AMachGameMode::Broadcast(AActor* Sender, const FString& Msg, FName Type)
{
	if (Msg.Len() > 0)
	{
		Super::Broadcast(Sender, Msg, Type);
	}
}

void AMachGameMode::UpdatePlayerVisibility()
{
	if (GetNumPlayers() == 0)
	{
		return;
	}

	AMachGameState* MachGameState = Cast<AMachGameState>(GameState);
	if (MachGameState == nullptr)
	{
		return;
	}

	TArray<AMachCharacter*> Characters;

	for (APlayerState* CurPlayerState : MachGameState->GetAllPlayers())
	{
		AMachPlayerState* MachPlayerState = Cast<AMachPlayerState>(CurPlayerState);
		if (MachPlayerState == nullptr || MachPlayerState->bIsDead)
		{
			continue;
		}

		AController* Controller = Cast<AController>(CurPlayerState->GetOwner());
		if (Controller == nullptr)
		{
			continue;
		}

		AMachCharacter* Character = Cast<AMachCharacter>(Controller->GetPawn());
		if (Character != nullptr)
		{
			Character->bIsVisibleToEnemy = false;
			Characters.Add(Character);
		}
	}

	for (AMachCharacter* Character : Characters)
	{
		for (AMachCharacter* OtherCharacter : Characters)
		{
			if (Character != OtherCharacter && !OtherCharacter->bIsVisibleToEnemy && Character->GetTeam() != OtherCharacter->GetTeam())
			{
				if (OtherCharacter->bIsBeingTracked)
				{
					OtherCharacter->bIsVisibleToEnemy = true;
					continue;
				}

				FCollisionQueryParams CollisionParms(FName(TEXT("VisibilityLOS")), true, Character);
				CollisionParms.AddIgnoredActor(OtherCharacter);
				bool bHit = GetWorld()->LineTraceTest(Character->GetActorLocation(), OtherCharacter->GetActorLocation(), ECC_Visibility, CollisionParms);
				if (!bHit)
				{
					OtherCharacter->bIsVisibleToEnemy = true;
				}
			}
		}
	}
}