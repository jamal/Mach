#pragma once

#include "GameFramework/Actor.h"
#include "MachUseTrigger.h"
#include "MachGameState.h"
#include "MachTechPoint.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachTechPoint : public AMachUseTrigger
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TSubobjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Testing")
	TSubobjectPtr<UPointLightComponent> PointLight1;

	/** Max number of concurrent users for this tech point */
	UPROPERTY(EditDefaultsOnly, Category = "Tech Point")
	uint32 MaxUsers;

	/** Time to capture the tech point in seconds */
	UPROPERTY(EditDefaultsOnly, Category = "Tech Point")
	float CaptureTime;

	/** Time to research an upgrade in seconds */
	UPROPERTY(EditDefaultsOnly, Category = "Tech Point")
	float ResearchTime;

	/** Time for the tech point to reset after a capture is interrupted */
	UPROPERTY(EditDefaultsOnly, Category = "Tech Point")
	float ResetTime;

	void Tick(float DeltaSeconds);

protected:

	float LightIntensity;
	float TimeToReset;

	UPROPERTY(Transient, Replicated)
	uint32 bIsCaptured : 1;

	/** Build progress */
	UPROPERTY(Transient, Replicated)
	float Progress;

	/** Team that is capturing or owns this point */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_Team)
	TEnumAsByte<ETeam::Type> Team;

	/** Team that is capturing or owns this point */
	UPROPERTY(Transient, Replicated)
	TEnumAsByte<ETeam::Type> CapturingTeam;

	UFUNCTION()
	void Capture(ETeam::Type CapturedByTeam);

	UFUNCTION()
	void ResetCapture();

	UFUNCTION()
	void OnRep_Team();

};
