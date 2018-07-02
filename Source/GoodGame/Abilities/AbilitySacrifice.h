#pragma once

#include "AbilityRay.h"
#include "AbilitySacrifice.generated.h"


UCLASS()
class GOODGAME_API AAbilitySacrifice : public AAbilityRay
{
	GENERATED_BODY()

public:
	AAbilitySacrifice();

protected:
	virtual void ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult) override;
	
};
