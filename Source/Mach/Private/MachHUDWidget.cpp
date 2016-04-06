// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachHUDWidget.h"

void UMachHUDWidget::AddChatLine(const FString& S, float T)
{
	if (T <= 0)
	{
		T = 10.f;
	}
	UE_LOG(LogTemp, Warning, TEXT("Adding chat line: %s, for %f"), *S, T);
	ChatHistory.Add(FChatMessage(S + "\n", T));
}