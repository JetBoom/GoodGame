#include "GoodGame.h"
#include "Weapon.h"
#include "HealthComponent.h"
#include "AbilityBattleRevive.h"


AAbilityBattleRevive::AAbilityBattleRevive()
{
	RayRange = 1000.f;
	RaySize = 8.f;
	RayDamage = 25.f;
	bHitDeadCharacters = true;
}

void AAbilityBattleRevive::ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult)
{
	AGoodGameCharacter* character = Cast<AGoodGameCharacter>(HitTarget);
	if (character) {
		if (character->IsDead()) {
			if (!character->HasStatusEffect(EStatusEffectType::ReviveSpell))
				character->AddStatusOfType(EStatusEffectType::ReviveSpell);
		}
		else {
			const AGoodGameCharacter* owner = (AGoodGameCharacter*)GetOwner();
			if (owner->ActiveWeapon)
				RayDamage *= owner->ActiveWeapon->WeaponMagnitude;

			character->GetHealthComponent()->GiveHealth(RayDamage, owner->Controller);
		}
	}
}