#include "GoodGame.h"
#include "SEProtection.h"


ASEProtection::ASEProtection()
{
}

void ASEProtection::OnOwnerTakeDamage_Implementation(float& DamageAmount, struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser, bool& bBlockDamage)
{
	if (!bDamageBlocked && DamageAmount >= 1.f) {
		bDamageBlocked = true;
		bInvalidStatus = true;
		DamageAmount = 0.f;
		Destroy();
	}
}
