// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "MachHelmet.h"
#include "MachHelmetTracking.generated.h"

UCLASS()
class MACH_API AMachHelmetTracking : public AMachHelmet
{
	GENERATED_BODY()
	
public:	
	float Range;

	// Sets default values for this actor's properties
	AMachHelmetTracking(const class FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void UseVision() override;
	
	/** Tell the server that we hit a trackable actor. */
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerTrackingHit(const FHitResult& Hit);
	virtual bool ServerTrackingHit_Validate(const FHitResult& Hit);
	virtual void ServerTrackingHit_Implementation(const FHitResult& Hit);

protected:
	UPROPERTY()
	class USceneComponent* RootScene;
	
};
