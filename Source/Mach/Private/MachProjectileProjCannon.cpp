// Copyright 2014 Rover Studios. All Rights Reserved.

#include "Mach.h"
#include "MachProjectileProjCannon.h"
#include "MachGameMode.h"
#include "MachCharacter.h"

AMachProjectileProjCannon::AMachProjectileProjCannon(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	ProjectileParticle = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("ProjectileParticle"));
	ProjectileParticle->AttachParent = RootComponent;

	HitParticleFriendly = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("HitParticleFriendly"));
	HitParticleFriendly->AttachParent = RootComponent;
	HitParticleFriendly->bAutoActivate = false;

	HitParticleEnemy = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("HitParticleEnemy"));
	HitParticleFriendly->AttachParent = RootComponent;
	HitParticleFriendly->bAutoActivate = false;
}

void AMachProjectileProjCannon::ProjectileHitActor(class AActor* Actor, const FHitResult& Hit)
{
	ProjectileParticle->DeactivateSystem();

	AMachGameMode* GameMode = Cast<AMachGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		bool bIsFriendly = GameMode->DamageIsFriendly(Instigator, Actor);
		if (bIsFriendly)
		{
			// Only heal players (not tech points and such)
			AMachCharacter* Character = Cast<AMachCharacter>(Actor);
			if (Character)
			{
				HitParticleFriendly->ActivateSystem();
				UE_LOG(LogTemp, Warning, TEXT("Healing a player"));
			}
		}
		else
		{
			HitParticleEnemy->ActivateSystem();
			UE_LOG(LogTemp, Warning, TEXT("Damaging a player"));
		}
	}
}

void AMachProjectileProjCannon::ProjectileHitComponent(class UPrimitiveComponent* Comp, const FHitResult& Hit)
{
	ProjectileParticle->DeactivateSystem();
}

void AMachProjectileProjCannon::ProjectileStopped(const FHitResult& Hit)
{
	ProjectileParticle->DeactivateSystem();
}
