#pragma once

#include "GameFramework/PlayerStart.h"
#include "MachGameState.h"
#include "MachPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachPlayerStart : public APlayerStart
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TEnumAsByte<ETeam::Type> Team;
};
