#pragma once

#include "GameFramework/Actor.h"
#include "MachUseTrigger.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachUseTrigger : public AActor
{
	GENERATED_UCLASS_BODY()

	UFUNCTION()
	void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void Tick(float DeltaSeconds);

protected:

	TArray<class AMachCharacter*> CharactersInTrigger;

	UPROPERTY(Transient, Replicated)
	uint32 bInUse : 1;

	/** Number of pawns actively using this trigger */
	UPROPERTY(Transient, Replicated)
	uint32 NumUsers;
	
};
