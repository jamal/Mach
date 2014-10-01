// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Mach.h"


AMachGameMode::AMachGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("/Game/Blueprints/MyCharacter"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*) PlayerPawnObject.Object->GeneratedClass;
	}

	HUDClass = AMachHUD::StaticClass();
	PlayerControllerClass = AMachPlayerController::StaticClass();
	PlayerStateClass = AMachPlayerState::StaticClass();
	GameStateClass = AMachGameState::StaticClass();

	bDelayedStart = false;
	bFriendlyFire = true;
	FriendlyFireDamagePercent = .5;

	SetTickableWhenPaused(true);
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

float AMachGameMode::DamageAmount(float Damage, const AActor* Causer, const AActor* Receiver) const
{
	if (bFriendlyFire && FriendlyFireDamagePercent < 1.f && DamageIsFriendly(Causer, Receiver))
	{
		return Damage * FriendlyFireDamagePercent;
	}

	return Damage;
}

void AMachGameMode::GameOver(ETeam::Type WinningTeam)
{
	UE_LOG(LogTemp, Warning, TEXT("Game is now over"));
	SetMatchState(MatchState::WaitingPostMatch);
	AMachGameState* State = Cast<AMachGameState>(GameState);
	if (State)
	{
		State->WinningTeam = WinningTeam;
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
		}
		KilledPlayerState->ScoreDeath(KillerState);
		
		// Record assists
		for (AMachPlayerState* AssistPlayer : KilledPlayerState->AssistPlayers)
		{
			if (AssistPlayer->PlayerId != KillerState->PlayerId)
			{
				AssistPlayer->ScoreAssist(KilledPlayerState);
			}
		}

		// TODO: Broadcast kill to all clients to display in HUD
	}
}

APlayerController* AMachGameMode::Login(UPlayer* NewPlayer, const FString& Portal, const FString& Options, const TSharedPtr<FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	ErrorMessage = GameSession->ApproveLogin(Options);
	if (!ErrorMessage.IsEmpty())
	{
		return NULL;
	}

	APlayerController* NewPlayerController = SpawnPlayerController(FVector::ZeroVector, FRotator::ZeroRotator);

	// Handle spawn failure.
	if (NewPlayerController == NULL)
	{
		UE_LOG(LogTemp, Log, TEXT("Couldn't spawn player controller of class %s"), PlayerControllerClass ? *PlayerControllerClass->GetName() : TEXT("NULL"));
		ErrorMessage = FString::Printf(TEXT("Failed to spawn player controller"));
		return NULL;
	}

	// Customize incoming player based on URL options
	InitNewPlayer(NewPlayerController, UniqueId, Options);

	// TODO: Login should talk to the world server to determine what team this player is assigned to
	// Dummy logic to assign players to teams
	AMachPlayerState* PlayerState = (AMachPlayerState*)NewPlayerController->PlayerState;
	if ((NumPlayersTeamA+NumPlayersTeamB) % 2 == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding player to team A"));
		PlayerState->SetTeam(ETeam::A);
		NumPlayersTeamA++;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding player to team B"));
		PlayerState->SetTeam(ETeam::B);
		NumPlayersTeamB++;
	}

	// Find a start spot.
	AActor* const StartSpot = FindPlayerStart(NewPlayerController, Portal);
	if (StartSpot == NULL)
	{
		ErrorMessage = FString::Printf(TEXT("Failed to find PlayerStart"));
		return NULL;
	}

	FRotator InitialControllerRot = StartSpot->GetActorRotation();
	InitialControllerRot.Roll = 0.f;
	NewPlayerController->SetInitialLocationAndRotation(StartSpot->GetActorLocation(), InitialControllerRot);
	NewPlayerController->StartSpot = StartSpot;

	// Register the player with the session
	GameSession->RegisterPlayer(NewPlayerController, UniqueId, HasOption(Options, TEXT("bIsFromInvite")));

	// Init player's name
	FString InName = ParseOption(Options, TEXT("Name")).Left(20);
	if (InName.IsEmpty())
	{
		InName = FString::Printf(TEXT("%s%i"), *DefaultPlayerName, NewPlayerController->PlayerState->PlayerId);
	}
	ChangeName(NewPlayerController, InName, false);

	// Set up spectating
	bool bSpectator = FCString::Stricmp(*ParseOption(Options, TEXT("SpectatorOnly")), TEXT("1")) == 0;
	if (bSpectator || MustSpectate(NewPlayerController))
	{
		NewPlayerController->StartSpectatingOnly();
		return NewPlayerController;
	}

	return NewPlayerController;
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

void AMachGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// Set a 15 second respawn timer
	UE_LOG(LogTemp, Warning, TEXT("Match is starting"));
	GetWorldTimerManager().SetTimer(this, &AMachGameMode::RespawnTimer, 15.0f, true);
}

void AMachGameMode::RespawnTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("Respawn Players"));
	TArray<class APlayerState*> Players = ((AMachGameState*)GameState)->GetAllPlayers();
	for (APlayerState* BasePlayer : Players)
	{
		AMachPlayerState* Player = (AMachPlayerState*)BasePlayer;
		if (Player->IsDead())
		{
			Player->SetIsDead(false);

			AController* Controller = Cast<AController>(Player->GetOwner());
			AActor* StartSpot = FindPlayerStart(Controller);

			Controller->SetPawn(SpawnDefaultPawnFor(Controller, StartSpot));

			if (Controller->GetPawn() != NULL)
			{
				Controller->Possess(Controller->GetPawn());
				Controller->ClientSetRotation(Controller->GetPawn()->GetActorRotation(), true);

				FRotator NewControllerRot = StartSpot->GetActorRotation();
				NewControllerRot.Roll = 0.f;
				Controller->SetControlRotation(NewControllerRot);

				SetPlayerDefaults(Controller->GetPawn());
				Controller->GetPawn()->PlayerState = Controller->PlayerState;
			}
		}
	}
}
