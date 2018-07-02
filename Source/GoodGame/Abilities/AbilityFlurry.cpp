#include "GoodGame.h"
#include "AbilityFlurry.h"


void AAbilityFlurry::ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult)
{
	if (HasAuthority())
		((AGoodGameCharacter*)HitTarget)->AddStatusOfType(EStatusEffectType::Flurry);
}
