#include "GoodGame.h"
#include "HealthComponent.h"
#include "GoodGameCharacter.h"
#include "ProjectileHealingBlast.h"


AProjectileHealingBlast::AProjectileHealingBlast()
{
}

void AProjectileHealingBlast::BeginPlay()
{
	Super::BeginPlay();
	
	Collider->OnComponentHit.AddDynamic(this, &AProjectileHealingBlast::OnHit);
}

void AProjectileHealingBlast::ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor)
{
	if (!HasAuthority())
		return;

	static FName NAME_ApplyRadialDamage = FName(TEXT("ApplyRadialDamage"));
	FCollisionQueryParams TraceParams(NAME_None, false, this);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, hitlocation, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(ProjectileRadius), TraceParams);

	UWorld* const World = GetWorld();

	AGoodGameCharacter* character;
	FVector TraceEnd;
	FHitResult OutHitResult;

	for (const auto& Overlap : Overlaps) {
		character = Cast<AGoodGameCharacter>(Overlap.GetActor());
		if (character) {
			TraceEnd = character->GetActorLocation();
			if (!World->LineTraceSingleByChannel(OutHitResult, hitlocation, TraceEnd, ECC_AOE, TraceParams) || OutHitResult.Actor == character) {
				character->GetHealthComponent()->GiveHealth(ProjectileDamage, GetInstigatorController());
			}
		}
	}
}
