

#include "GoodGame.h"
#include "HealthComponent.h"
#include "GoodGamePawn.h"


AGoodGamePawn::AGoodGamePawn(const FObjectInitializer& ObjectInitializer)
{
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	HealthComponent->HealthMax = HealthComponent->HealthBase = 100.f;
	HealthComponent->HealthRate = 1.f;
}

void AGoodGamePawn::BeginPlay()
{
	Super::BeginPlay();
}

float AGoodGamePawn::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// This converts falloff damage to direct damage.
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Only server handles damage.
	if (!HasAuthority())
		return DamageAmount;

	// Pawns have no stamina or mana

	// Take the health. IHasHealth has an optional OnKilled.
	if (HealthComponent)
		HealthComponent->TakeHealth(DamageAmount);

	return DamageAmount;
}

bool AGoodGamePawn::OnKilled(AActor* attacker, AActor* inflictor)
{
	UE_LOG(LogTemp, Log, TEXT("Pawn Killed"));

	return true;
}

void AGoodGamePawn::OnRevived()
{
	UE_LOG(LogTemp, Log, TEXT("Pawn Revived"));
}
