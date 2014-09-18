#include "Mach.h"
#include "Example.h"


AExample::AExample(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AExample::Call()
{
	UE_LOG(LogTemp, Log, TEXT("Example Call"));

	switch (Role)
	{
	case ROLE_None:
		UE_LOG(LogTemp, Log, TEXT("ROLE_None"));
		break;
	case ROLE_SimulatedProxy:
		UE_LOG(LogTemp, Log, TEXT("ROLE_SimulatedProxy"));
		break;
	case ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Log, TEXT("ROLE_AutonomousProxy"));
		break;
	case ROLE_Authority:
		UE_LOG(LogTemp, Log, TEXT("ROLE_Authority"));
		break;
	case ROLE_MAX:
		UE_LOG(LogTemp, Log, TEXT("ROLE_MAX"));
		break;
	}

	ServerCall();
}

bool AExample::ServerCall_Validate()
{
	return true;
}

void AExample::ServerCall_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Server call implementation"));
}
