// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Mach.h"
#include "MachHUD.h"
#include "MachPlayerState.h"
#include "MachCharacter.h"

AMachHUD::AMachHUD(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/Textures/Crosshair"));
	CrosshairTex = CrosshiarTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UFont> TheFontOb(TEXT("Font'/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField'"));
	Font = (UFont*)TheFontOb.Object;
}


void AMachHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
										   (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)) );

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

	DrawNetMode();
	DrawPlayerTeam();
	DrawPlayerHealth();
}

void AMachHUD::DrawNetMode()
{
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	FText message;
	switch (GetNetMode())
	{
	case NM_Client:
		message = FText::FromString("Client");
		break;
	case NM_ListenServer:
		message = FText::FromString("Listen Server");
		break;
	case NM_DedicatedServer:
		message = FText::FromString("Dedicated Server");
		break;
	case NM_Standalone:
		message = FText::FromString("Standalone");
		break;
	}

	const FVector2D TextDrawPosition(Center.X, Center.Y - 100.f);
	FCanvasTextItem TextItem(TextDrawPosition, message, Font, FColor::White);
	TextItem.bCentreX = true;
	Canvas->DrawItem(TextItem);
}

void AMachHUD::DrawPlayerTeam()
{
	FText message;

	if (PlayerOwner->PlayerState != NULL)
	{
		AMachPlayerState* PlayerState = (AMachPlayerState*)PlayerOwner->PlayerState;

		if (PlayerState->GetTeam() == ETeam::A)
		{
			message = FText::FromString("Team A");
		}
		else if (PlayerState->GetTeam() == ETeam::B)
		{
			message = FText::FromString("Team B");
		}

		const FVector2D TextDrawPosition(50.0f, 10.0f);
		FCanvasTextItem TextItem(TextDrawPosition, message, Font, FColor::White);
		TextItem.bCentreX = true;
		Canvas->DrawItem(TextItem);
	}
}

void AMachHUD::DrawPlayerHealth()
{
	FText message;

	AMachCharacter* Character = (AMachCharacter*)PlayerOwner->GetPawn();
	if (Character != NULL)
	{
		message = FText::Format(FText::FromString("{0} HP / {1} SP"), FText::AsNumber(Character->Health), FText::AsNumber(Character->Shield));

		const FVector2D TextDrawPosition(100.0f, Canvas->ClipY - 30.0f);
		FCanvasTextItem TextItem(TextDrawPosition, message, Font, FColor::White);
		TextItem.bCentreX = true;
		Canvas->DrawItem(TextItem);
	}
}
