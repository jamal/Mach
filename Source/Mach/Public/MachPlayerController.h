#pragma once

#include "GameFramework/PlayerController.h"
#include "MachGameState.h"
#include "MachPlayerController.generated.h"

UENUM(PlayerArmor, BlueprintType)
namespace EPlayerArmor
{
	enum Type
	{
		None,
		Light,
		Stealth,
		Medium,
		Heavy,
	};
}

namespace EPlayerArmor
{
	inline EPlayerArmor::Type FromInt(int32 Value)
	{
		switch (Value)
		{
		case 1: return EPlayerArmor::Light; break;
		case 2: return EPlayerArmor::Stealth; break;
		case 3: return EPlayerArmor::Medium; break;
		case 4: return EPlayerArmor::Heavy; break;
		default: return EPlayerArmor::None;
		}
	};

	inline int32 ToInt(EPlayerArmor::Type Value)
	{
		switch (Value)
		{
		case EPlayerArmor::Light: return 1; break;
		case EPlayerArmor::Stealth: return 2; break;
		case EPlayerArmor::Medium: return 3; break;
		case EPlayerArmor::Heavy: return 4; break;
		default: return 0;
		}
	};
}

UENUM(PlayerPrimaryWeapon, BlueprintType)
namespace EPlayerPrimaryWeapon
{
	enum Type
	{
		None,
		Rifle,
		Shotgun,
		EnergyGun,
		GrenadeLauncher,
		ProjCannon,
	};
}

namespace EPlayerPrimaryWeapon
{
	inline EPlayerPrimaryWeapon::Type FromInt(int32 Value) {
		switch (Value)
		{
		case 1: return EPlayerPrimaryWeapon::Rifle; break;
		case 2: return EPlayerPrimaryWeapon::Shotgun; break;
		case 3: return EPlayerPrimaryWeapon::EnergyGun; break;
		case 4: return EPlayerPrimaryWeapon::GrenadeLauncher; break;
		case 5: return EPlayerPrimaryWeapon::ProjCannon; break;
		default: return EPlayerPrimaryWeapon::None;
		}
	};

	inline int32 ToInt(EPlayerPrimaryWeapon::Type Value) {
		switch (Value)
		{
		case EPlayerPrimaryWeapon::Rifle: return 1; break;
		case EPlayerPrimaryWeapon::Shotgun: return 2; break;
		case EPlayerPrimaryWeapon::EnergyGun: return 3; break;
		case EPlayerPrimaryWeapon::GrenadeLauncher: return 4; break;
		case EPlayerPrimaryWeapon::ProjCannon: return 5; break;
		default: return 0;
		}
	};
}

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerData)
	TEnumAsByte<EPlayerArmor::Type> Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerData)
	TEnumAsByte<EPlayerPrimaryWeapon::Type> PrimaryWeapon;
};

/**
 * 
 */
UCLASS()
class MACH_API AMachPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()
	
	FName NAME_Say;
	FName NAME_TeamSay;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Player data required for spawning a new player, armor, gear, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	FPlayerData PlayerPawnData;

	UFUNCTION(BlueprintCallable, Category = Input)
	void SetIgnoreInput(bool bInput);

	UFUNCTION(BlueprintCallable, Category = Input)
	void SetMousePosition(int32 x, int32 y);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Player)
	void SetPlayerData(FPlayerData NewPlayerData);

	bool CanRestartPlayer();

	void PawnPendingDestroy(APawn* Pawn) override;

	UFUNCTION(reliable, client)
	void ClientSetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation);

	/** Console commands */
	UFUNCTION(exec)
	void LightArmor();
	UFUNCTION(exec)
	void MediumArmor();
	UFUNCTION(exec)
	void StealthArmor();
	UFUNCTION(exec)
	void HeavyArmor();

	UFUNCTION(exec)
	void Rifle();
	UFUNCTION(exec)
	void Shotgun();
	UFUNCTION(exec)
	void GrenadeLauncher();
	UFUNCTION(exec)
	void ProjCannon();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Player)
	void SwapArmor(EPlayerArmor::Type NewArmor);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Player)
	void SwapWeapon(EPlayerPrimaryWeapon::Type NewWeapon);

	/** Send a broadcast chat message. */
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = Chat)
	void SendMessage(const FString& S);

	UFUNCTION(BlueprintImplementableEvent, Category = Chat)
	void OnClientMessage(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime);

	void SetInputModeGameAndUI();
	void SetInputModeGameOnly();

	ETeam::Type GetTeam();

protected:
	class AMachCharacter* MachCharacter;
	uint32 bIgnoreInput : 1;
	uint32 bEquipmentPicked : 1;

	TSubclassOf<class UWidgetBlueprint> EquipmentWidgetClass;
	class UBlueprintWidget* EquipmentWidget;

	TSubclassOf<AActor> PreviewActorClass;
	class ACameraActor* PreviewCamera;
	class AActor* PreviewPawn;

	AMachCharacter* GetMachCharacter();

	void PostInitializeComponents();
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

	void Crouch();
	void UnCrouch();
	void StartSprinting();
	void StopSprinting();

	class AMachHUD* GetMachHUD() const;

	void OnShowScoreboard();
	void OnHideScoreboard();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void StartJump();
	void StopJump();
	void StartStealth();
	void OnStartFire();
	void OnStopFire();
	void OnStartSecondaryFire();
	void OnStopSecondaryFire();
	void Equip1();
	void Equip2();
	void Equip3();
	void OnReload();
	void OnStopUse();
	void OnStartUse();
	void StartMovementSpecial();
	void StopMovementSpecial();
	void UseVision();

	/** Override ClientTeamMessage to display it in our chat widget. */
	virtual void ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime) override;
};
