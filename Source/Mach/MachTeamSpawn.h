#pragma once

#include "GameFramework/Actor.h"
#include "MachGameState.h"
#include "MachTeamSpawn.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachTeamSpawn : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float BaseHealth;

	UPROPERTY(VisibleDefaultsOnly, Category = Object)
	TSubobjectPtr<class USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Object)
	TSubobjectPtr<class UBoxComponent> BoxComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	TEnumAsByte<ETeam::Type> Team;

	virtual void PostInitializeComponents() override;

	bool ShouldTakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) const;
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	void ReceiveAnyDamage(float Damage, const class UDamageType* DamageType, class AController* EventInstigator, class AActor* DamageCauser);

private:

	UPROPERTY(Transient, Replicated)
	float Health;
};
