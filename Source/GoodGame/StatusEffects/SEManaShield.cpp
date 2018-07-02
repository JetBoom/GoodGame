#include "GoodGame.h"
#include "StaminaManaComponent.h"
#include "SEManaShield.h"

ASEManaShield::ASEManaShield()
{
}

void ASEManaShield::OnOwnerTakeDamage_Implementation(float& DamageAmount, struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser, bool& bBlockDamage)
{
	// Take 50% damage and 40% of the original damage to mana.
	if (DamageAmount >= 1.f) {
		UStaminaManaComponent* StaminaMana = ((AGoodGameCharacter*)GetOwner())->GetStaminaManaComponent();
		
		StaminaMana->TakeMana(DamageAmount * 0.4f);

		DamageAmount *= 0.5f;

		// Nullified if user runs out of mana.
		if (StaminaMana->GetMana() <= 0.f)
			Destroy();
	}
}
