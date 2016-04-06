// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachHUD.h"
#include "MachHUDWidget.h"
#include "MachCharacter.h"
#include "MachGameState.h"
#include "MachPlayerState.h"

AMachHUD::AMachHUD(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AMachHUD::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AMachGameState>(UGameplayStatics::GetGameState(GetWorld()));

	MinimapMI = UMaterialInstanceDynamic::Create(MinimapMaterial, nullptr);
	MinimapTexture = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), 256, 256);
	MinimapTexture->ClearColor = FLinearColor(0.f, 0.f, 0.f, 0.f);
	MinimapTexture->OnCanvasRenderTargetUpdate.AddDynamic(this, &AMachHUD::UpdateMinimapTexture);
	MinimapMI->SetTextureParameterValue(FName(TEXT("MinimapTexture")), MinimapTexture);
}

void AMachHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HUDWidget != nullptr)
	{
		for (int i = 0; i < HUDWidget->ChatHistory.Num(); ++i)
		{

			HUDWidget->ChatHistory[i].LifeTime -= DeltaTime;
			if (HUDWidget->ChatHistory[i].LifeTime <= 0.f)
			{
				HUDWidget->ChatHistory.RemoveAt(i);
			}
		}
	}

	if (MinimapTexture != nullptr)
	{
		APlayerController* PC = Cast<APlayerController>(GetOwner());
		if (PC != nullptr)
		{
			MinimapMI->SetScalarParameterValue("Rotation", PC->GetControlRotation().Yaw / 360.f - 0.75f);
		}
		MinimapTexture->UpdateResource();
	}
}

void AMachHUD::AddChatLine(const FString& Message, float LifeTime)
{
	// Very simple and dumb implementation
	if (HUDWidget != nullptr)
	{
		//HUDWidget->ChatHistory += "\n" + Message;
		HUDWidget->AddChatLine(Message, LifeTime);
	}
}

void AMachHUD::UpdateMinimapTexture(UCanvas* C, int32 Width, int32 Height)
{
	// TODO: Minimap should have its own class

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC == nullptr)
	{
		return;
	}
	
	AMachCharacter* Owner = Cast<AMachCharacter>(PC->GetPawn());
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get controller owner"));
		return;
	}

	if (!GameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to draw minimap with no GameState"));
		return;
	}

	C->DrawColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

	if (MI_Background)
	{
		float XL = MI_Background->GetSurfaceWidth();
		float YL = MI_Background->GetSurfaceHeight();
		C->DrawTile(MI_Background, 0, 0, XL, YL, 0, 0, XL, YL, BLEND_Translucent);
	}

	if (MI_Enemy && MI_Friendly)
	{
		UTexture2D* CurrentTexture = nullptr;

		// TODO: Change this to use a sphere collider or something more efficient
		// TODO: Suppport things that aren't characters
		for (TActorIterator<AMachCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (Owner != *ActorItr)
			{
				FVector Rel = Owner->GetActorLocation() - ActorItr->GetActorLocation();
				if (Rel.Size() >= 5000.f)
				{
					// Skip players that are too far
					continue;
				}

				// Always show friendly players
				if (ActorItr->GetTeam() == Owner->GetTeam())
				{
					CurrentTexture = MI_Friendly;
				}
				else if (ActorItr->bIsVisibleToEnemy)
				{
					CurrentTexture = MI_Enemy;
				}
				else
				{
					continue;
				}

				if (CurrentTexture != nullptr)
				{
					float XL = CurrentTexture->GetSurfaceWidth();
					float YL = CurrentTexture->GetSurfaceHeight();
					float DrawX = Width / 2 - (Rel.X * (Width / 2) / 5000.f) - (XL / 2);
					float DrawY = Height / 2 - (Rel.Y * (Height / 2) / 5000.f) - (YL / 2);
					C->DrawTile(CurrentTexture, DrawX, DrawY, XL, YL, 0, 0, XL, YL, BLEND_Translucent);
				}
			}
		}
	}

	if (MI_Player)
	{
		float XL = MI_Player->GetSurfaceWidth();
		float YL = MI_Player->GetSurfaceHeight();
		C->DrawTile(MI_Player, Width / 2 - (XL/2), Height / 2 - (YL/2), XL, YL, 0, 0, XL, YL, BLEND_Translucent);
	}
}

void AMachHUD::ShowScoreboard_Implementation(bool bVisible)
{

}