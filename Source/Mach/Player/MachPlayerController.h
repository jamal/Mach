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

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UFUNCTION(BlueprintCallable, Category = Input)
	void SetIgnoreInput(bool bInput);

	UFUNCTION(BlueprintCallable, Category = Input)
	void SetMousePosition(int32 x, int32 y);

protected:
	virtual void SetupInputComponent() override;

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	class AMachHUD* GetMachHUD() const;

	void OnShowScoreboard();
	void OnHideScoreboard();
};
