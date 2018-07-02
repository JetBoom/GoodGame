#include "GoodGame.h"
#include "RayEffect.h"
#include "AbilityRay.h"


AAbilityRay::AAbilityRay() {}

void AAbilityRay::OnAbilityEnd_Implementation(const EStopCastingReason Reason)
{
	Super::OnAbilityEnd_Implementation(Reason);

	if (Reason == EStopCastingReason::Success && HasAuthority()) {
		AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
		if (owner) {
			FVector eyepos;
			FRotator eyerot;
			owner->GetShootTransform(eyepos, eyerot);

			const FVector normal = eyerot.Vector();
			const FVector endpos = eyepos + normal * RayRange;

			const FCollisionQueryParams params = FCollisionQueryParams(NAME_None, false, owner);

			TArray<FHitResult> HitResults;

			if (RaySize > 0.f) {
				static FCollisionShape shape;
				shape.SetSphere(RaySize);

				if (bHitMultiple)
					GetWorld()->SweepMultiByChannel(HitResults, eyepos, endpos, FQuat::Identity, ECC_AOE, shape, params);
				else {
					FHitResult HitResult;
					GetWorld()->SweepSingleByChannel(HitResult, eyepos, endpos, FQuat::Identity, ECC_Visibility, shape, params);
					HitResults.Add(HitResult);
				}
			}
			else if (bHitMultiple)
				GetWorld()->LineTraceMultiByChannel(HitResults, eyepos, endpos, ECC_AOE, params);
			else {
				FHitResult HitResult;
				GetWorld()->LineTraceSingleByChannel(HitResult, eyepos, endpos, ECC_Visibility, params);
				HitResults.Add(HitResult);
			}

			OnRayTraceResult(HitResults, eyepos, endpos, normal);
		}
	}
}

void AAbilityRay::OnRayTraceResult_Implementation(const TArray<FHitResult>& HitResults, const FVector& eyepos, const FVector& endpos, const FVector& normal)
{
	float effectDistance = RayRange;

	for (const FHitResult& HitResult : HitResults) {
		AActor* HitTarget = HitResult.Actor.Get();
		const bool isPawn = HitTarget && HitTarget->IsA<APawn>();
		bool isApplied = isPawn || !bOnlyHitPawns;

		if (isApplied && HitTarget && !bHitDeadCharacters) {
			AGoodGameCharacter* charTarget = Cast<AGoodGameCharacter>(HitTarget);
			if (charTarget && charTarget->IsDead())
				isApplied = false;
		}

		RayHitLogic(HitResult, eyepos, endpos, normal, HitTarget, isApplied);

		if (!bHitMultiple || HitResult.bBlockingHit)
			effectDistance = FMath::Min(effectDistance, HitResult.Distance);

		RayHitEffects(HitResult, eyepos, endpos, normal, HitTarget, isApplied);
	}

	RayCastEffects(eyepos, normal, effectDistance);
}

void AAbilityRay::RayHitLogic(const FHitResult& HitResult, const FVector& eyepos, const FVector& endpos, const FVector& normal, AActor* HitTarget, const bool& isApplied)
{
	if (HitTarget && isApplied)
		ApplyRayHit(HitTarget, HitResult);
}

void AAbilityRay::RayHitEffects(const FHitResult& HitResult, const FVector& eyepos, const FVector& endpos, const FVector& normal, AActor* HitTarget, const bool& isApplied)
{
	const bool isPawn = HitTarget && HitTarget->IsA<APawn>();
	if (RayHitEffect && (HitResult.bBlockingHit || bHitMultiple) && (isApplied || bHitEffectEvenIfNotApplied)) {
		AEffect* eff = GetWorld()->SpawnActor<AEffect>(RayHitEffect, isPawn ? HitTarget->GetActorLocation() : HitResult.ImpactPoint, isPawn ? FRotator(0.f, HitTarget->GetActorRotation().Yaw, 0.f) : HitResult.ImpactNormal.Rotation());
		if (eff) {
			eff->SetReplicates(true);
			if (isPawn) {
				static const FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
				eff->AttachToActor(HitTarget, rules);
			}
		}
	}
}

void AAbilityRay::RayCastEffects(const FVector& eyepos, const FVector& normal, const float& distance)
{
	if (RayEffect) {
		ARayEffect* rayEff = GetWorld()->SpawnActor<ARayEffect>(RayEffect, eyepos, normal.Rotation());
		if (rayEff) {
			rayEff->SetReplicates(true);
			rayEff->RayDistance = distance;
			if (!IsNetMode(NM_DedicatedServer))
				rayEff->UpdateParticleParameters();
		}
	}
}

void AAbilityRay::ApplyRayHit(AActor* HitTarget, const FHitResult& HitResult)
{
	if (RayDamage > 0.f && RayDamageType) {
		((AGoodGameCharacter*)GetOwner())->ModifyOutgoingDamage(RayDamage, RayDamageType, true);

		UGameplayStatics::ApplyPointDamage(HitTarget, RayDamage, HitResult.Normal, HitResult, (AController*)GetOwner(), this, RayDamageType);
	}
}
