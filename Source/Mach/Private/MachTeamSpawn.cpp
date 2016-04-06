#include "Mach.h"
#include "MachTeamSpawn.h"
#include "MachGameMode.h"

AMachTeamSpawn::AMachTeamSpawn(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	BoxComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BoxComponent"));
	BoxComponent->InitBoxExtent(FVector(15.f, 15.f, 15.f));
	BoxComponent->SetCollisionProfileName("BlockAll");
	RootComponent = BoxComponent;

	Mesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("TeamSpawn"));
	Mesh->AttachParent = BoxComponent;

	bCanBeDamaged = true;
	bReplicates = true;
}

void AMachTeamSpawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Role == ROLE_Authority)
	{
		Health = BaseHealth;
		UE_LOG(LogTemp, Warning, TEXT("Spawning new team spawn with health %.2f"), Health);
	}
}

bool AMachTeamSpawn::ShouldTakeDamage(float Damage, FDamageEvent const & DamageEvent, AController* EventInstigator, AActor* DamageCauser) const {
	if (Role < ROLE_Authority || Damage == 0.f || !GetWorld()->GetAuthGameMode())
	{
		return false;
	}

	AMachGameMode* GameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		return GameMode->ShouldTakeDamage(Damage, DamageEvent, EventInstigator->GetCharacter(), this);
	}

	return true;
}

float AMachTeamSpawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser))
	{
		return 0.f;
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AMachTeamSpawn::ReceiveAnyDamage(float Damage, const class UDamageType* DamageType, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Role == ROLE_Authority && Damage > 0.f && GetWorld()->GetAuthGameMode())
	{
		AMachGameMode* GameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			Damage = GameMode->DamageAmount(Damage, EventInstigator->GetCharacter(), this);
		}

		if (Damage > 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn took %.2f damage"), Damage);
			Health -= Damage;
			if (Health <= 0.f) {
				UE_LOG(LogTemp, Warning, TEXT("Spawn has been killed"));
				GameMode->GameOver( ((Team == ETeam::A) ? ETeam::B : ETeam::A) );
			}
		}
	}
}

void AMachTeamSpawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMachTeamSpawn, Health);
}
