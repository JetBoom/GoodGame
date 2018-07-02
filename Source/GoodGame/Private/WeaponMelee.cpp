#include "GoodGame.h"
#include "StaminaManaComponent.h"
#include "WeaponMelee.h"


AWeaponMelee::AWeaponMelee()
{
	bShouldEverShowHUDProgress = false;

	WeaponSlot = EWeaponSlots::Melee;
}

void AWeaponMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Only server and local client should tick this.
	if (!IsOwnerOrAuthority())
		return;

	if (StartAttackTime <= SMALL_NUMBER)
		return;

	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());

	if (!owner) {
		ResetAttack();
		return;
	}

	const float curtime = WORLD_TIME;
	const float swingFraction = (curtime - StartAttackTime) / (FinishAttackTime - StartAttackTime);

	if (!bHitProcessed && swingFraction >= SwingTimeHitFraction) {
		bHitProcessed = true;

		ForceNetUpdate();

		if (HasAuthority()) {
			TArray<FHitResult> Hits;

			FCollisionQueryParams params = FCollisionQueryParams(NAME_None, false);
			params.AddIgnoredActor(owner);
			params.bIgnoreBlocks = true;

			static FCollisionShape shape;
			shape.SetSphere(MeleeRange);

			const float fudge = owner->GetCapsuleComponent()->GetScaledCapsuleRadius();
			const FVector normal = owner->GetBaseAimRotation().Vector();
			const FVector swingpos = owner->GetCapsuleComponent()->GetCenterOfMass() - normal * fudge; // owner->GetPawnViewLocation();
			const FVector start = swingpos - normal * (MeleeRange * 2.01f);

			GetWorld()->SweepMultiByProfile(Hits, start, swingpos + normal * fudge, FQuat(), FName("MeleeHitDetection"), shape, params);

			//DrawDebugSphere(GetWorld(), swingpos, MeleeRange, 32, FColor::Yellow, false, 2.f, 0, 1.f);

			float damage = BaseDamage;
			owner->ModifyOutgoingDamage(damage, MeleeDamageType, true);
			
			const auto attacker = owner->GetController();

			AActor* hitactor;
			FVector hittonormal;
			FVector impactpoint;
			UPrimitiveComponent* component;
			
			for (const auto& hit : Hits) {
				//if (FVector::DistSquared(hit.ImpactPoint, start) > 8.f)
				component = hit.GetComponent();
				hitactor = hit.Actor.Get();

				if (!component || !hitactor || !hitactor->IsA<APawn>() || hitactor->IsA<AGoodGameCharacter>() && ((AGoodGameCharacter*)hitactor)->IsDead())
					continue;

				impactpoint = hit.ImpactPoint;
				impactpoint.Z = swingpos.Z;// +normal.Z * component->Bounds.SphereRadius;
				hittonormal = impactpoint - swingpos;
				hittonormal.Normalize();

				//DrawDebugPoint(GetWorld(), impactpoint, 8.f, FColor::Yellow, false, 2.f);

				if (FVector::DotProduct(normal, hittonormal) >= MeleeMinDot && UGGStatics::IsVisible(swingpos, component, owner))
					UGameplayStatics::ApplyPointDamage(hitactor, damage, hit.ImpactNormal, hit, attacker, this, MeleeDamageType);
			}
		}
	}

	if (!bShouldPlaySwingSound && swingFraction >= SwingTimeSoundFraction) {
		bShouldPlaySwingSound = true;

		ForceNetUpdate();

		PlaySwingSound();
	}

	if (swingFraction >= 1.0f)
		ResetAttack();
}

void AWeaponMelee::PlaySwingSound()
{
	if (!IsNetMode(NM_DedicatedServer)) {
		const auto owner = GetOwner();
		if (owner) {
			if (SwingSound)
				UGGStatics::SpawnSoundOnActor(SwingSound, owner);

			if (FMath::RandHelper(2) == 0) {
				AGoodGameCharacter* cowner = Cast<AGoodGameCharacter>(GetOwner());
				if (cowner)
					UGGStatics::SpawnSoundOnActor(cowner->AggroSound, cowner);
			}
		}
	}
}

void AWeaponMelee::OnAttackPressed_Implementation()
{
	if (IsBusy())
		return;

	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
	if (owner && !owner->IsDead() && !owner->CastingAbility && owner->GetStaminaManaComponent()->TakeStaminaIfHas(StaminaCost)) {
		const float curtime = WORLD_TIME;
		if (curtime >= owner->GlobalCooldown) {
			bHitProcessed = false;

			SetAttackTimes(curtime, curtime + BaseSwingTime / owner->AttackSpeedMultiplier / WeaponSpeed);
		}
	}
}

void AWeaponMelee::OnStartAttack_Implementation()
{
	Super::OnStartAttack_Implementation();
}

void AWeaponMelee::ResetAttack()
{
	Super::ResetAttack();

	bHitProcessed = false;
	bShouldPlaySwingSound = false;

	ForceNetUpdate();
}

void AWeaponMelee::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponMelee::OnRep_HitProcessed()
{
	if (bHitProcessed) {
		const auto owner = GetOwner();
		if (owner)
			UGGStatics::SpawnSoundOnActor(SwingSound, owner);
	}
}

void AWeaponMelee::OnRep_ShouldPlaySwingSound()
{
	if (bShouldPlaySwingSound)
		PlaySwingSound();
}

void AWeaponMelee::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponMelee, bHitProcessed, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AWeaponMelee, bShouldPlaySwingSound, COND_SkipOwner);
}
