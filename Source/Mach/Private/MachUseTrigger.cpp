#include "Mach.h"
#include "MachUseTrigger.h"
#include "MachCharacter.h"

AMachUseTrigger::AMachUseTrigger(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bReplicates = true;
	bInUse = false;
}

void AMachUseTrigger::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role == ROLE_Authority)
	{
		AMachCharacter* Character = Cast<AMachCharacter>(OtherActor);
		if (OtherActor && OtherActor != this && OtherComp && Character)
		{
			UE_LOG(LogTemp, Warning, TEXT("Adding character to trigger"));
			CharactersInTrigger.AddUnique(Character);
		}
	}
}

void AMachUseTrigger::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority)
	{
		AMachCharacter* Character = Cast<AMachCharacter>(OtherActor);
		if (OtherActor && OtherActor != this && OtherComp && Character)
		{
			UE_LOG(LogTemp, Warning, TEXT("Removing character from trigger"));
			CharactersInTrigger.Remove(Character);
		}
	}
}

void AMachUseTrigger::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Role == ROLE_Authority)
	{
		bInUse = false;

		for (AMachCharacter* Character : CharactersInTrigger)
		{
			if (Character->IsUsePressed())
			{
				bInUse = true;
				NumUsers++;
			}
		}
	}
}

void AMachUseTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachUseTrigger, bInUse);
	DOREPLIFETIME(AMachUseTrigger, NumUsers);
}
