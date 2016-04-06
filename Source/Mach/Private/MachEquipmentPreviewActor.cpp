// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachEquipmentPreviewActor.h"


AMachEquipmentPreviewActor::AMachEquipmentPreviewActor(class FObjectInitializer const & ObjectInitializer) 
	: Super(ObjectInitializer)
{
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("PreviewScene"));
	RootComponent = SceneComponent;

	BackgroundBillboard = ObjectInitializer.CreateDefaultSubobject<UBillboardComponent>(this, TEXT("PreviewBillboard"));
	BackgroundBillboard->AttachTo(RootComponent);
	BackgroundBillboard->bIsScreenSizeScaled = true;
	static ConstructorHelpers::FObjectFinder<UTexture2D> BillboardSprite(TEXT("Texture2D'/Game/Textures/Test_Menu_Billboard.Test_Menu_Billboard'"));
	BackgroundBillboard->SetSprite(BillboardSprite.Object);
	BackgroundBillboard->RelativeLocation = FVector(500.f, 0.f, 0.f);
	
	CameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("PreviewCamera"));
	CameraComponent->AttachTo(RootComponent);
	CameraComponent->RelativeLocation = FVector(0.f, 0.f, 0.f);

	LightComponent = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, TEXT("PreviewLight"));
	LightComponent->Intensity = 1000.f;
	LightComponent->bVisible = true;
	LightComponent->SetLightColor(FLinearColor::White);

	BodyMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PreviewBodyMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BodyMeshSkeleton(TEXT("SkeletalMesh'/Game/ProtoCharacters/SK_PrototypeM.SK_PrototypeM'"));
	BodyMesh->AttachTo(RootComponent);
	BodyMesh->SetSkeletalMesh(BodyMeshSkeleton.Object);
	BodyMesh->RelativeLocation = FVector(100.f, -40.f, -50.f);
	BodyMesh->RelativeRotation = FRotator(0.f, 60.f, 0.f);
	BodyMesh->SetWorldScale3D(FVector(0.4f, 0.4f, 0.4f));

	WeaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PreviewWeaponMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WeaponMeshSkeleton(TEXT("SkeletalMesh'/Game/ProtoCharacters/SK_PrototypeM.SK_PrototypeM'"));
	WeaponMesh->AttachTo(BodyMesh);
	WeaponMesh->SetSkeletalMesh(WeaponMeshSkeleton.Object);
	//WeaponMesh->RelativeLocation = FVector(100.f, -60.f, -50.f);
}
