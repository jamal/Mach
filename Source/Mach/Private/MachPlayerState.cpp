#include "Mach.h"
#include "MachPlayerState.h"

AMachPlayerState::AMachPlayerState(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bAlwaysRelevant = true;
	bReplicates = true;
	bIsSpectator = false;
	bOnlySpectator = false;
}

TEnumAsByte<ETeam::Type> AMachPlayerState::GetTeam()
{
	return Team;
}

void AMachPlayerState::SetTeam(ETeam::Type NewTeam)
{
	Team = NewTeam;
}

void AMachPlayerState::SetIsDead(bool isDead)
{
	bIsDead = isDead;
}

bool AMachPlayerState::IsDead()
{
	return bIsDead;
}

void AMachPlayerState::ScoreKill(AMachPlayerState* Victim)
{
	UE_LOG(LogTemp, Warning, TEXT("Kill for %d"), PlayerId);
	NumKills++;
}

void AMachPlayerState::ScoreDeath(AMachPlayerState* Killer)
{
	UE_LOG(LogTemp, Warning, TEXT("Death for %d"), PlayerId);
	NumDeaths++;
}

void AMachPlayerState::ScoreAssist(AMachPlayerState* Victim)
{
	UE_LOG(LogTemp, Warning, TEXT("Assist for %d"), PlayerId);
	NumAssists++;
}

void AMachPlayerState::AddAssistPlayer(AMachPlayerState* PlayerState)
{
	AssistPlayers.AddUnique(PlayerState);
}

void AMachPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachPlayerState, Team);
	DOREPLIFETIME(AMachPlayerState, bIsDead);
	DOREPLIFETIME(AMachPlayerState, NumKills);
	DOREPLIFETIME(AMachPlayerState, NumDeaths);
	DOREPLIFETIME(AMachPlayerState, NumAssists);
	DOREPLIFETIME(AMachPlayerState, TechTree);
}
