#include "GoodGame.h"
#include "StatusEffectBlockDamage.h"

void AStatusEffectBlockDamage::OnOwnerTakeDamage_Implementation(float& DamageAmount, struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser, bool& bBlockDamage)
{
	bBlockDamage = true;
}
