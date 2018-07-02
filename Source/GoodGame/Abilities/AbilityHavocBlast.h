#pragma once

#include "AbilityRay.h"
#include "AbilityHavocBlast.generated.h"


UCLASS()
class GOODGAME_API AAbilityHavocBlast : public AAbilityRay
{
	GENERATED_BODY()
	
protected:
	virtual void RayHitLogic(const FHitResult& HitResult, const FVector& eyepos, const FVector& endpos, const FVector& normal, AActor* HitTarget, const bool& isApplied) override;
	virtual void RayHitEffects(const FHitResult& HitResult, const FVector& eyepos, const FVector& endpos, const FVector& normal, AActor* HitTarget, const bool& isApplied) override;

};
