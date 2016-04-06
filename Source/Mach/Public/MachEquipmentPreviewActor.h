// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "MachEquipmentPreviewActor.generated.h"

/**
 * 
 */
UCLASS()
class MACH_API AMachEquipmentPreviewActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:

	UPROPERTY(VisibleDefaultsOnly, Category = Preview)
	class USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Preview)
	class UBillboardComponent* BackgroundBillboard;

	UPROPERTY(VisibleDefaultsOnly, Category = Preview)
	class USkeletalMeshComponent* BodyMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Preview)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Preview)
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Preview)
	class ULightComponent* LightComponent;
	
};
