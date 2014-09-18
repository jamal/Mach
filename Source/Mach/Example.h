#pragma once

#include "GameFramework/Actor.h"
#include "Example.generated.h"

/**
 * 
 */
UCLASS()
class AExample : public AActor
{
	GENERATED_UCLASS_BODY()

	UFUNCTION()
	void Call();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCall();
	
};
