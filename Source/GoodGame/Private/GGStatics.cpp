#include "GoodGame.h"
#include "GGStatics.h"

UGGStatics::UGGStatics(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

UAudioComponent* UGGStatics::SpawnSoundOnActor(USoundCue* SoundCue, AActor* Actor)
{
	if (Actor)
		return UGameplayStatics::SpawnSoundAttached(SoundCue, Actor->GetRootComponent(), NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, true);

	return nullptr;
}

bool UGGStatics::IsVisible(const FVector& origin, const UPrimitiveComponent* component, const AActor* ignore) {
	FVector TraceEnd;
	FHitResult OutHitResult;

	const auto& bounds = component->Bounds;
	const auto World = component->GetWorld();

	static FName NAME_ApplyRadialDamage = FName(TEXT("ApplyRadialDamage"));
	FCollisionQueryParams TraceParams(NAME_ApplyRadialDamage, false, ignore);

	TraceEnd = bounds.Origin;

	if (!World->LineTraceSingleByChannel(OutHitResult, origin, TraceEnd, ECC_AOE, TraceParams) || OutHitResult.GetComponent() == component)
		return true;

	const FBox box = bounds.GetBox();

	if (!World->LineTraceSingleByChannel(OutHitResult, origin, (box.GetClosestPointTo(origin) * 4.f + TraceEnd) / 5.f, ECC_AOE, TraceParams) || OutHitResult.GetComponent() == component)
		return true;

	return false;
}

float UGGStatics::InternalTakeRadialDamage(AActor* victim, float Damage, FRadialDamageEvent const& RadialDamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	float ActualDamage = Damage;

	FVector ClosestHitLoc(0);

	// find closest component
	// @todo, something more accurate here to account for size of components, e.g. closest point on the component bbox?
	// @todo, sum up damage contribution to each component?
	float ClosestHitDistSq = MAX_FLT;
	FVector nearest;
	for (int32 HitIdx = 0; HitIdx < RadialDamageEvent.ComponentHits.Num(); ++HitIdx)
	{
		FHitResult const& Hit = RadialDamageEvent.ComponentHits[HitIdx];
		const float d = Hit.GetComponent()->GetDistanceToCollision(RadialDamageEvent.Origin, nearest);
		const float DistSq = d == 0.f ? 0.f : (nearest - RadialDamageEvent.Origin).SizeSquared();
		if (DistSq < ClosestHitDistSq)
		{
			ClosestHitDistSq = DistSq;
			ClosestHitLoc = Hit.ImpactPoint;
		}
	}

	float const RadialDamageScale = RadialDamageEvent.Params.GetDamageScale(FMath::Sqrt(ClosestHitDistSq));

	ActualDamage = FMath::Lerp(RadialDamageEvent.Params.MinimumDamage, ActualDamage, FMath::Max(0.f, RadialDamageScale));

	return ActualDamage;
}
