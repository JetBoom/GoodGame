#include "GoodGame.h"
#include "GoodGameDamageType.h"


bool UGoodGameDamageType::CalculateBackDamage(float& DamageAmount, struct FDamageEvent const& DamageEvent, class AActor* const DamageReceiver)
{
	// Only direct damage can apply critical damage on the back.
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		auto pde = (const FPointDamageEvent&)DamageEvent;

		FVector impactNorm = pde.HitInfo.ImpactNormal;
		impactNorm.Z = 0.f;
		impactNorm.Normalize();

		FVector actorNorm = DamageReceiver->GetActorForwardVector();
		actorNorm.Z = 0.f;
		actorNorm.Normalize();

		if (FVector::DotProduct(impactNorm, actorNorm) < 0.1f /*-0.333f*/) {
			DamageAmount *= 1.3333f;
			return true;
		}
	}

	return false;
}

FVector UGoodGameDamageType::CalculateDamageImpulseOrigin(struct FDamageEvent const& DamageEvent, class AActor* const DamageReceiver, class AActor* const DamageCauser)
{
	FVector origin;

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
		origin = ((const FRadialDamageEvent&)DamageEvent).Origin;
	else if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		origin = ((const FPointDamageEvent&)DamageEvent).HitInfo.Location;
	else if (DamageCauser)
		origin = DamageCauser->GetActorLocation();
	else
		origin = DamageReceiver->GetActorLocation();

	// Move the origin a bit lower if it's slightly above our us, making it so we still get launched from being hit in the head for example.
	if (DamageReceiver->IsA<ACharacter>()) {
		FVector recLocation;
		FVector extent;
		DamageReceiver->GetActorBounds(true, recLocation, extent);

		float idealZ = recLocation.Z - extent.Z; // actor lower Z
		if (origin.Z > idealZ)
			origin.Z -= FMath::Min(origin.Z - idealZ, 100.f);
	}

	return origin;
}
