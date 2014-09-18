#include "Mach.h"
#include "MachPlayerState.h"


AMachPlayerState::AMachPlayerState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

TEnumAsByte<ETeam::Type> AMachPlayerState::GetTeam()
{
	return Team;
}

void AMachPlayerState::SetTeam(ETeam::Type NewTeam)
{
	Team = NewTeam;
}

void AMachPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachPlayerState, Team);
	DOREPLIFETIME(AMachPlayerState, bIsDead);
}

void AMachPlayerState::SetIsDead(bool isDead)
{
	bIsDead = isDead;
}

bool AMachPlayerState::IsDead()
{
	return bIsDead;
}