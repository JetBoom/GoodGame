#include "GoodGame.h"
#include "AbilityRayWithStatusEffect.h"


void AAbilityRayWithStatusEffect::ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult)
{
	Super::ApplyRayHit(HitTarget, HitResult);

	AGoodGameCharacter* charTarget = Cast<AGoodGameCharacter>(HitTarget);
	if (charTarget && !charTarget->IsDead())
		charTarget->AddStatusOfType(RayStatusEffect);
}
