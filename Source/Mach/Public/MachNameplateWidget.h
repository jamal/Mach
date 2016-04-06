// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MachNameplateWidget.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachNameplateWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category = Nameplate)
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = Nameplate)
	class AMachCharacter* Character;
	
};

