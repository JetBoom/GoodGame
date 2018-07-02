#include "GoodGame.h"
#include "ProjectilePoP.h"


void AProjectilePoP::BeginPlay()
{
	Super::BeginPlay();

	//Collider->OnComponentHit.AddDynamic(this, &AProjectilePoP::OnHit);
}

void AProjectilePoP::ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor)
{
	Super::ProjectileHitLogic(hitlocation, hitnormal, SweepResult, OtherActor);

	static FActorSpawnParameters spawnparams;
	spawnparams.Instigator = Instigator;
	spawnparams.Owner = Instigator;

	AAffectorArea* affector = GetWorld()->SpawnActor<AAffectorArea>(AffectorClass, hitlocation, hitnormal.ToOrientationRotator(), spawnparams);
	if (affector)
		affector->UpdateFromProjectile(this);
}
