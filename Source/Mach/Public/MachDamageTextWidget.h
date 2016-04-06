// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MachDamageTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:

	UPROPERTY(BlueprintReadOnly, Category = DamageTextWidget)
	float Damage;

	UPROPERTY(BlueprintReadOnly, Category = DamageTextWidget)
	AActor* DamagedActor;
	
};
