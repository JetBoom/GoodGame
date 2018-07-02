#include "GoodGame.h"
#include "StatusEffectType.h"
#include "AbilityAcidRock.h"


AAbilityAcidRock::AAbilityAcidRock() {}

void AAbilityAcidRock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Target) {
		TimeRemaining -= DeltaTime;

		if (TimeRemaining <= 0.f) {
			if (!Target->IsDead()) {
				Target->AddStatusOfType(EStatusEffectType::GuardBreak);
				Super::ApplyRayHit(Target, SavedHitResult);
			}

			Target = nullptr;
		}
	}
}

void AAbilityAcidRock::ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult)
{
	if (HasAuthority()) {
		Target = (AGoodGameCharacter*)HitTarget;
		SavedHitResult = HitResult;
		TimeRemaining = 2.0f;
	}
}
