// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"
#include "MachHUD.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachHUD : public AHUD
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = HUD)
	class UMachHUDWidget* HUDWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Minimap)
	class UMaterialInterface* MinimapMaterial;

	/** material instance showing the minimap */
	UPROPERTY(BlueprintReadOnly, Category = Minimap)
	class UMaterialInstanceDynamic* MinimapMI;

	/** render target for the minimap */
	UPROPERTY(BlueprintReadOnly, Category = Minimap)
	class UCanvasRenderTarget2D* MinimapTexture;

	// Minimap Textures
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Minimap)
	UTexture2D* MI_Background;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Minimap)
	UTexture2D* MI_Player;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Minimap)
	UTexture2D* MI_Enemy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Minimap)
	UTexture2D* MI_Friendly;

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	/** Very basic implementation of chat. */
	void AddChatLine(const FString& Message, float LifeTime);

	UFUNCTION()
	void UpdateMinimapTexture(UCanvas* C, int32 Width, int32 Height);

	UFUNCTION(BlueprintNativeEvent, Category=HUD)
	void ShowScoreboard(bool bVisible);
	
protected:
	class AMachGameState* GameState;
};
