#pragma once

#include "AbilityRay.h"
#include "AbilityAcidRock.generated.h"


UCLASS()
class GOODGAME_API AAbilityAcidRock : public AAbilityRay
{
	GENERATED_BODY()
	
public:
	AAbilityAcidRock();

protected:
	UPROPERTY()
	AGoodGameCharacter* Target;

	UPROPERTY()
	float TimeRemaining = 1.f;

	UPROPERTY()
	FHitResult SavedHitResult;

	virtual void ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult) override;
	virtual void Tick(float DeltaTime) override;

};
