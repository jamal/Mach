#include "Mach.h"
#include "MachTechPoint.h"
#include "MachPlayerState.h"
#include "MachCharacter.h"
#include "MachGameMode.h"
#include "MachTechPointListenerInterface.h"

AMachTechPoint::AMachTechPoint(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	MaxUsers = 2;
	CaptureTime = 5.0f;
	ResearchTime = 40.f;
	ResetTime = 10.f;
	Progress = 0.f;
	bIsCaptured = false;
	Team = ETeam::None;
	LightIntensity = 20000.f;

	SphereComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(250.0f);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AMachTechPoint::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AMachTechPoint::OnEndOverlap);

	RootComponent = SphereComponent;

	// Adding a light component to test with
	PointLight1 = PCIP.CreateDefaultSubobject<UPointLightComponent>(this, TEXT("PointLight1"));
	PointLight1->Intensity = LightIntensity;
	PointLight1->bVisible = true;
	PointLight1->SetLightColor(FLinearColor::White);
	PointLight1->AttachParent = SphereComponent;
}

void AMachTechPoint::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Pulsing light animation while point is captured
	if (Progress > 0.f && Progress < 100.f && bIsCaptured)
	{
		float Frequency = 1.f;
		PointLight1->Intensity = (0.5f * (1 + FMath::Sin(2 * PI * Frequency * GetWorld()->TimeSeconds))) * LightIntensity;
		GetWorld()->Scene->UpdateLightColorAndBrightness(PointLight1);
	}

	if (Role == ROLE_Authority && bInUse && Progress < 100.f)
	{
		// Check what teams are present in the trigger
		// If there are multiple teams then we can't proceed
		uint32 bTeamConflict = false;
		ETeam::Type TeamInTrigger = ETeam::None;
		for (AMachCharacter* Character : CharactersInTrigger)
		{
			if (TeamInTrigger != ETeam::None && TeamInTrigger != Character->GetTeam())
			{
				bTeamConflict = true;
				break;
			}

			TeamInTrigger = Character->GetTeam();
		}

		// Can only build this tech point IF:
		//  - Only 1 team is present in the trigger
		//  - The node is not captured OR the enemy team has it captured
		if (!bTeamConflict && (!bIsCaptured || TeamInTrigger != Team))
		{
			// If the point is neutral, and the enemy team had made progress then we will reset to capture for our team
			if (!bIsCaptured && Progress > 0.f && CapturingTeam != TeamInTrigger)
			{
				ResetCapture();
			}
			
			GetWorldTimerManager().SetTimer(this, &AMachTechPoint::ResetCapture, ResetTime, false);

			CapturingTeam = TeamInTrigger;
			float Multiplier = (NumUsers < MaxUsers) ? NumUsers : MaxUsers;
			Progress += (DeltaSeconds / CaptureTime) * 100 * Multiplier;
			if (Progress >= 100.f)
			{
				Capture(TeamInTrigger);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Building this tech point, progress=%.2f"), Progress);
			}
		}
	}
}

void AMachTechPoint::Capture(ETeam::Type CapturedByTeam)
{
    bool bCaptured; // Bool to hold whether we just captured or lost a tech point
    
	FLinearColor LightColor = FLinearColor::White;
	if (bIsCaptured && Team != CapturedByTeam)
	{
        bCaptured = false;
        
		// Reset the status to neutral, and continue
		AMachGameMode* MachGameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
		if (MachGameMode)
		{
			MachGameMode->UncaptureTechPoint(Team);
		}

		bIsCaptured = false;
		Progress = 0.f;
		Team = ETeam::None;

		UE_LOG(LogTemp, Warning, TEXT("Tech point was reset to neutral"));
	}
	else if (Team != CapturedByTeam)
	{
        bCaptured = true;
        
		AMachGameMode* MachGameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
		if (MachGameMode)
		{
			MachGameMode->CaptureTechPoint(CapturedByTeam);
		}

		Progress = 0.f;
		bIsCaptured = true;
		Team = CapturedByTeam;

		switch (CapturedByTeam)
		{
		case ETeam::A:
			LightColor = FLinearColor::Red;
			UE_LOG(LogTemp, Warning, TEXT("Tech point was captured by team A"));
			break;
		case ETeam::B:
			LightColor = FLinearColor::Blue;
			UE_LOG(LogTemp, Warning, TEXT("Tech point was captured by team B")); 
			break;
		}
	}
    
    // Debugging with lights
	if (PointLight1 != NULL)
	{
		PointLight1->Intensity = LightIntensity;
		PointLight1->SetLightColor(LightColor);
	}
    
    // Notify all TechPointListeners
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
        IMachTechPointListenerInterface* TechPointListener = Cast<IMachTechPointListenerInterface>(*ActorItr);
        if (TechPointListener)
        {
            if (bCaptured)
            {
                TechPointListener->OnTechPointCaptured();
            }
            else
            {
                TechPointListener->OnTechPointLost();
            }
        }
	}
}

void AMachTechPoint::ResetCapture()
{
	if (Role == ROLE_Authority && Progress > 0.f)
	{
		Progress = 0.f;
		PointLight1->Intensity = LightIntensity;
		GetWorld()->Scene->UpdateLightColorAndBrightness(PointLight1);
	}
}

void AMachTechPoint::OnRep_Team(ETeam::Type OldTeam)
{
	if (PointLight1 != NULL)
	{
		PointLight1->Intensity = LightIntensity;

		switch (Team)
		{
		case ETeam::A:
			PointLight1->SetLightColor(FLinearColor::Red);
			break;
		case ETeam::B:
			PointLight1->SetLightColor(FLinearColor::Blue);
			break;
		default:
			PointLight1->SetLightColor(FLinearColor::White);
		}
	}
}

void AMachTechPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachTechPoint, Progress);
	DOREPLIFETIME(AMachTechPoint, Team);
	DOREPLIFETIME(AMachTechPoint, CapturingTeam);
	DOREPLIFETIME(AMachTechPoint, bIsCaptured);
}