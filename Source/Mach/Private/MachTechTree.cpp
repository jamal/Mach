// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachTechTree.h"

AMachTechTree::AMachTechTree(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Team = ETeam::None;
	Power = 0;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AMachTechTree::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachTechTree, Team);
	DOREPLIFETIME(AMachTechTree, Power);
}
