#pragma once

#include "AbilityRay.h"
#include "AbilityFlurry.generated.h"


UCLASS()
class GOODGAME_API AAbilityFlurry : public AAbilityRay
{
	GENERATED_BODY()
	
public:
	virtual void ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult) override;
	
};
