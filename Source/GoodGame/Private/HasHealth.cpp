#include "GoodGame.h"
#include "HasHealth.h"


// This function does not need to be modified.
UHasHealth::UHasHealth(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

// Add default functionality here for any IHasHealth functions that are not pure virtual.
float IHasHealth::GetHealth() const
{
	return 0.f;
}

float IHasHealth::GetMaxHealth() const
{
	return 100.f;
}

bool IHasHealth::OnKilled(AActor* attacker, AActor* inflictor)
{
	return true;
}

void IHasHealth::OnRevived()
{
}
