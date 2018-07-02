#include "GoodGame.h"
#include "AffectorAreaDamage.h"


void AAffectorAreaDamage::DoAffect(AActor* actor)
{
	if (Damage > 0.f && HasAuthority())
		actor->TakeDamage(Damage, FDamageEvent(DamageType), GetInstigatorController(), this);
}

void AAffectorAreaDamage::UpdateFromProjectile(const AProjectile* projectile)
{
	Damage = projectile->ProjectileDamage;
	DamageType = projectile->ProjectileDamageType;
	SetRadius(projectile->ProjectileRadius);
}
