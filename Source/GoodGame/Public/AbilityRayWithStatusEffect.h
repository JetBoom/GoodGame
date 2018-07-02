#pragma once

#include "AbilityRay.h"
#include "StatusEffectType.h"
#include "AbilityRayWithStatusEffect.generated.h"


UCLASS()
class GOODGAME_API AAbilityRayWithStatusEffect : public AAbilityRay
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RayWithStatusEffect)
	EStatusEffectType RayStatusEffect;
	
	virtual void ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult) override;

};
