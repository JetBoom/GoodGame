#pragma once

#include "AbilityRay.h"
#include "AbilityBattleRevive.generated.h"


UCLASS()
class GOODGAME_API AAbilityBattleRevive : public AAbilityRay
{
	GENERATED_BODY()
	
public:
	AAbilityBattleRevive();

protected:
	virtual void ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult) override;
	
	
};
