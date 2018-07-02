#include "GoodGame.h"
#include "ProjectileExplosiveArrow.h"


void AProjectileExplosiveArrow::BeginPlay()
{
	OriginalDamage = ProjectileDamage;

	Super::BeginPlay();
}

void AProjectileExplosiveArrow::ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor)
{
	ProjectileDamage /= 2.f;
	Super::ProjectileHitLogic(hitlocation, hitnormal, SweepResult, OtherActor);

	ProjectileDamage = OriginalDamage;
	ProjectileDamageType = ProjectileDamageType2;
	if (Instigator) {
		const AGoodGameCharacter* characterInstigator = Cast<AGoodGameCharacter>(Instigator);
		if (characterInstigator)
			characterInstigator->ModifyOutgoingDamage(ProjectileDamage, ProjectileDamageType, true);
	}

	ProjectileDamage /= 2.f;
	Super::ProjectileHitLogic(hitlocation, hitnormal, SweepResult, OtherActor);
}
