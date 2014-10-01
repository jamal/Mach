// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once 

#include "MachHUD.generated.h"

UCLASS()
class AMachHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	void ToggleScoreboard();
	void ShowScoreboard(bool bEnable);

private:
	/** Crosshair asset pointer */
	UTexture2D* CrosshairTex;

	UFont* Font;

	bool bIsScoreboardVisible;

	/** Scoreboard widget. */
	TSharedPtr<class SMachScoreboardWidget> ScoreboardWidget;

	/** Scoreboard widget overlay. */
	TSharedPtr<class SOverlay>	ScoreboardWidgetOverlay;

	/** Scoreboard widget container - used for removing */
	TSharedPtr<class SWidget> ScoreboardWidgetContainer;

	virtual void PostInitializeComponents() override;

	void DrawNetMode();
	void DrawPlayerTeam();
	void DrawPlayerHealth();
	void DrawGameOver();

};
