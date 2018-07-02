#include "GoodGame.h"
#include "AbilityHavocBlast.h"


void AAbilityHavocBlast::RayHitLogic(const FHitResult& HitResult, const FVector& eyepos, const FVector& endpos, const FVector& normal, AActor* HitTarget, const bool& isApplied)
{
	if (HitResult.bBlockingHit)
		UGameplayStatics::ApplyRadialDamage(GetWorld(), RayDamage, HitResult.ImpactPoint + HitResult.ImpactNormal, 600.f, RayDamageType, TArray<AActor*>(), this, (AController*)GetOwner(), true, ECC_AOE);
}

void AAbilityHavocBlast::RayHitEffects(const FHitResult& HitResult, const FVector& eyepos, const FVector& endpos, const FVector& normal, AActor* HitTarget, const bool& isApplied)
{
	if (HitResult.bBlockingHit && RayHitEffect) {
		AEffect* eff = GetWorld()->SpawnActor<AEffect>(RayHitEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
		if (eff)
			eff->SetReplicates(true);
	}
}
