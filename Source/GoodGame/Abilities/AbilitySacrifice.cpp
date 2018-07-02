#include "GoodGame.h"
#include "HealthComponent.h"
#include "StaminaManaComponent.h"
#include "Weapon.h"
#include "AbilitySacrifice.h"


AAbilitySacrifice::AAbilitySacrifice() {}

void AAbilitySacrifice::ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult)
{
	if (RayDamage > 0.f) {
		AGoodGameCharacter* character = Cast<AGoodGameCharacter>(HitTarget);
		if (character) {
			const AGoodGameCharacter* owner = (AGoodGameCharacter*)GetOwner();
			float healthToGive = RayDamage;
			if (owner->ActiveWeapon)
				healthToGive *= owner->ActiveWeapon->WeaponMagnitude;

			character->GetHealthComponent()->GiveHealth(healthToGive, owner->Controller);
			character->GetStaminaManaComponent()->GiveStamina(healthToGive * 0.25f);
			character->GetStaminaManaComponent()->GiveMana(healthToGive * 0.25f);
		}
	}
}
