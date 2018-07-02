#include "GoodGame.h"
#include "UnrealNetwork.h"
#include "HasHealth.h"
#include "GoodGamePlayerState.h"
#include "HealthComponent.h"


UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bReplicates = true;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() < NM_Client) {
		HealthUpdateTime = WORLD_TIME;

		if (HealthBase == 0.f)
			SetHealth(HealthMax);
	}
}

void UHealthComponent::SetHealth(float health)
{
	if (bIsDead)
		return;

	if (health <= 0.f) {
		health = 0.f;
		SetIsDead(true);
	}
	/*else if (bIsDead && health > 0.f)
		bIsDead = false;*/

	HealthBase = health;
	HealthUpdateTime = WORLD_TIME;
}

float UHealthComponent::GetHealth() const
{
	if (bIsDead)
		return 0.f;

	if (HealthRate <= 0.f)
		return HealthBase;

	const float health = HealthBase + HealthRate * (WORLD_TIME - HealthUpdateTime);

	if (health > HealthMax)
		return HealthMax;

	return health;
}

void UHealthComponent::SetMaxHealth(const float& health)
{
	HealthMax = FMath::Max(health, 1.f);
}

float UHealthComponent::GetMaxHealth() const
{
	return HealthMax;
}

void UHealthComponent::TakeHealth(const float& health, AActor* attacker, AActor* inflictor)
{
	if (attacker && attacker->IsA<AGoodGameCharacter>())
		SetLastAttacker(attacker, inflictor);

	SetHealth(GetHealth() - health);
}

void UHealthComponent::GiveHealth(const float& health, AController* healer)
{
	const float oldHealth = GetHealth();

	SetHealth(oldHealth + health);

	if (bIsDead)
		return;

	const float healthChange = GetHealth() - oldHealth;
	if (healthChange <= 0.f)
		return;

	AActor* owner = GetOwner();

	const APawn* pawn = Cast<APawn>(owner);
	
	if (pawn) {
		AGoodGamePlayerState* ownerState = Cast<AGoodGamePlayerState>(pawn->PlayerState);
		if (ownerState)
			ownerState->SendDamageNumber(owner, health, false, true);
	}

	if (healer && (!pawn || pawn->PlayerState != healer->PlayerState)) {
		AGoodGamePlayerState* state = Cast<AGoodGamePlayerState>(healer->PlayerState);
		if (state)
			state->SendDamageNumber(owner, health, false, true);
	}
}

void UHealthComponent::SetIsDead(const bool& dead)
{
	if (dead != bIsDead) {
		bIsDead = dead;

		IHasHealth* hashealth = Cast<IHasHealth>(GetOwner());
		if (hashealth) {
			if (dead) {
				if (LastAttackerSetTime > 0.f && GetWorld()->TimeSeconds < LastAttackerSetTime + 5.f)
					hashealth->OnKilled(LastAttacker.Get(), LastInflictor.Get());
				else
					hashealth->OnKilled(nullptr, nullptr);
			}
			else
				hashealth->OnRevived();
		}
	}
}

void UHealthComponent::SetLastAttacker(AActor* attacker, AActor* inflictor)
{
	if (attacker) {
		LastAttackerSetTime = GetWorld()->TimeSeconds;
		LastAttacker = attacker;
		LastInflictor = inflictor;
	}
	else
		LastAttackerSetTime = 0.f;
}

bool UHealthComponent::IsDead() const
{
	return bIsDead;
}

void UHealthComponent::OnRep_IsDead()
{
	IHasHealth* hashealth = Cast<IHasHealth>(GetOwner());
	if (hashealth) {
		if (bIsDead) {
			/*if (LastAttackerSetTime > 0.f && GetWorld()->TimeSeconds < LastAttackerSetTime + 5.f)
				hashealth->OnKilled(LastAttacker.Get(), LastInflictor.Get());
			else*/
				hashealth->OnKilled(nullptr, nullptr);
		}
		else
			hashealth->OnRevived();
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, HealthBase);
	DOREPLIFETIME(UHealthComponent, HealthMax);
	DOREPLIFETIME(UHealthComponent, HealthUpdateTime);
	DOREPLIFETIME(UHealthComponent, HealthRate);
	DOREPLIFETIME(UHealthComponent, bIsDead);
}
