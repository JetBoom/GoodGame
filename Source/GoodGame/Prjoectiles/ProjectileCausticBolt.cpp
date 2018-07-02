#include "GoodGame.h"
#include "ProjectileCausticBolt.h"


void AProjectileCausticBolt::ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor)
{
	Super::ProjectileHitLogic(hitlocation, hitnormal, SweepResult, OtherActor);

	if (OtherActor) {
		auto character = Cast<AGoodGameCharacter>(OtherActor);
		if (character) {
			auto status = character->AddStatusOfType(EStatusEffectType::CausticBolt, 4.f, false);
			if (status && status->NumberOfStacks < 5)
				status->SetNumberOfStacks(status->NumberOfStacks + 1);
		}
	}
}
