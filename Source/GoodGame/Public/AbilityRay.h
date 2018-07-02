#pragma once

#include "Ability.h"
#include "RayEffect.h"
#include "GoodGameDamageType.h"
#include "AbilityRay.generated.h"

UCLASS()
class GOODGAME_API AAbilityRay : public AAbility
{
	GENERATED_BODY()
	
public:
	AAbilityRay();

	// Provides damage and effects by default but can be overridden or extended.
	UFUNCTION(BlueprintNativeEvent, Category = Ray)
	void OnRayTraceResult(const TArray<FHitResult>& HitResults, const FVector& eyepos, const FVector& endpos, const FVector& normal);

	virtual void OnAbilityEnd_Implementation(const EStopCastingReason Reason) override;
	
protected:
	// Some standard logic for applying damage to a single target.
	virtual void ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult);

	// More advanced logic. Calls ApplyRayHit.
	virtual void RayHitLogic(const FHitResult& HitResult, const FVector& eyepos, const FVector& endpos, const FVector& normal, AActor* HitTarget, const bool& isApplied);

	// GFX for hits.
	virtual void RayHitEffects(const FHitResult& HitResult, const FVector& eyepos, const FVector& endpos, const FVector& normal, AActor* HitTarget, const bool& isApplied);

	// GFX for the ray itself.
	virtual void RayCastEffects(const FVector& eyepos, const FVector& normal, const float& distance);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	float RayRange = 2500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	float RaySize = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	float RayDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	TSubclassOf<UGoodGameDamageType> RayDamageType;

	// The effect played as the "trace" for the ray.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	TSubclassOf<ARayEffect> RayEffect;

	// The effect played on an actor that was hit with this ray.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	TSubclassOf<AEffect> RayHitEffect;

	// The ray will only damage and hit pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	bool bOnlyHitPawns = true;

	// The hit effect will spawn even if we don't hit a valid object. Will still not spawn if it hits air.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	bool bHitEffectEvenIfNotApplied = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	bool bHitDeadCharacters = false;

	// Ray will pierce targets to hit behind them.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ray)
	bool bHitMultiple = false;

};
