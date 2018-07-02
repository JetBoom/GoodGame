#include "GoodGame.h"
#include "StatusEffectDOT.h"


AStatusEffectDOT::AStatusEffectDOT()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 1.f;
}

void AStatusEffectDOT::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(99999.f);
	EndTime = WORLD_TIME + (Stacks - 1) * GetActorTickInterval();
}

void AStatusEffectDOT::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DoDOT();
}

void AStatusEffectDOT::DoDOT()
{
	if (!HasAuthority())
		return;

	if (Stacks <= 0) {
		Destroy();
		return;
	}

	--Stacks;

	AActor* owner = GetOwner();
	if (owner && DamageType)
		owner->TakeDamage(Damage, FDamageEvent(DamageType), Instigator ? ((APawn*)Instigator)->GetController() : NULL, this);
}
