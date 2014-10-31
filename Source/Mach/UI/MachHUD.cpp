#include "Mach.h"
#include "UI/Widgets/SMachScoreboardWidget.h"

AMachHUD::AMachHUD(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/Textures/Crosshair"));
	CrosshairTex = CrosshiarTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UFont> TheFontOb(TEXT("Font'/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField'"));
	Font = (UFont*)TheFontOb.Object;
}

void AMachHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bIsScoreboardVisible = false;
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
	Canvas->DrawItem(TileItem);

	//DrawNetMode();
	//DrawPlayerTeam();
	//DrawPlayerHealth();
	//DrawGameOver();
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

	const FVector2D TextDrawPosition(Center.X, 10.0f);
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

void AMachHUD::DrawGameOver()
{
	AMachCharacter* Character = (AMachCharacter*)PlayerOwner->GetPawn();
	AMachGameState* GameState = GetWorld()->GetGameState<AMachGameState>();
	if (GameState && Character)
	{
		if (GameState && GameState->HasMatchEnded() && GameState->WinningTeam != ETeam::None)
		{
			FText message;
			if (Character->GetTeam() == GameState->WinningTeam)
			{
				message = FText::FromString("You Have Won!");
			}
			else
			{
				message = FText::FromString("You Have Lost");
			}

			const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
			const FVector2D TextDrawPosition(Center.X, Center.Y - 100.f);
			FCanvasTextItem TextItem(TextDrawPosition, message, Font, FColor::White);
			TextItem.bCentreX = true;
			Canvas->DrawItem(TextItem);
		}
	}
}

void AMachHUD::ToggleScoreboard()
{
	ShowScoreboard(!bIsScoreboardVisible);
}

void AMachHUD::ShowScoreboard(bool bEnable)
{
	APlayerController* PC = Cast<APlayerController>(PlayerOwner);

	if (bIsScoreboardVisible == bEnable)
	{
		return;
	}

	bIsScoreboardVisible = bEnable;
	
	if (bIsScoreboardVisible)
	{
		SAssignNew(ScoreboardWidgetOverlay, SOverlay)
			+ SOverlay::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(50))
			[
				SAssignNew(ScoreboardWidget, SMachScoreboardWidget)
				.PCOwner(TWeakObjectPtr<APlayerController>(PlayerOwner))
			];

		GEngine->GameViewport->AddViewportWidgetContent(
			SAssignNew(ScoreboardWidgetContainer, SWeakWidget)
			.PossiblyNullContent(ScoreboardWidgetOverlay));
	}
	else if (ScoreboardWidgetContainer.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(ScoreboardWidgetContainer.ToSharedRef());
	}
}