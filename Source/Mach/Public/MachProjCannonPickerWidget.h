// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MachWeaponProjCannon.h"
#include "MachProjCannonPickerWidget.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API UMachProjCannonPickerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	class AMachWeaponProjCannon* Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjCannon)
	TEnumAsByte<EProjCannonBuildable::Type> SelectedBuildable;


};
