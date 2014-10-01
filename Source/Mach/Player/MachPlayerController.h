#pragma once

#include "GameFramework/PlayerController.h"
#include "MachPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void SetupInputComponent() override;

	class AMachHUD* GetMachHUD() const;

	void OnShowScoreboard();
	void OnHideScoreboard();
};
