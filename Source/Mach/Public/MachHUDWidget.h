// Copyright 2014 Rover Studios. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MachHUDWidget.generated.h"


USTRUCT(BlueprintType)
struct FChatMessage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chat)
	FString Message;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Chat)
	float LifeTime;

	FChatMessage()
	{
	}

	FChatMessage(const FString& S, float T)
		: Message(S), LifeTime(T)
	{

	}
};


/**
 * 
 */
UCLASS()
class MACH_API UMachHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = Chat)
	TArray<FChatMessage> ChatHistory;

	void AddChatLine(const FString& S, float T);
};
