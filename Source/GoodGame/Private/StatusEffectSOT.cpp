#include "GoodGame.h"
#include "GoodGameCharacter.h"
#include "HealthComponent.h"
#include "StaminaManaComponent.h"
#include "StatusEffectSOT.h"

AStatusEffectSOT::AStatusEffectSOT()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 1.f;
}

void AStatusEffectSOT::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(99999.f);
	EndTime = WORLD_TIME + (Stacks - 1) * GetActorTickInterval();
}

void AStatusEffectSOT::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DoSOT();
}

void AStatusEffectSOT::DoSOT()
{
	if (!HasAuthority())
		return;

	if (Stacks <= 0) {
		Destroy();
		return;
	}

	--Stacks;

	AGoodGameCharacter* charowner = (AGoodGameCharacter*)GetOwner();

	if (charowner) {
		if (HealthPerStack > 0.f)
			charowner->GetHealthComponent()->GiveHealth(HealthPerStack, GetInstigatorController());
		if (StaminaPerStack > 0.f)
			charowner->GetStaminaManaComponent()->GiveStamina(StaminaPerStack);
		if (ManaPerStack > 0.f)
			charowner->GetStaminaManaComponent()->GiveMana(ManaPerStack);
	}
}
