#include "GoodGame.h"
#include "ProjectileBegone.h"


AProjectileBegone::AProjectileBegone()
{
	Collider->InitSphereRadius(8.f);
}

void AProjectileBegone::BeginPlay()
{
	Super::BeginPlay();

	Collider->OnComponentHit.AddDynamic(this, &AProjectileBegone::OnHit);
}

void AProjectileBegone::ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor)
{
	Super::ProjectileHitLogic(hitlocation, hitnormal, SweepResult, OtherActor);

	FRotator rot = GetActorRotation();
	if (SweepResult.Actor == nullptr || !SweepResult.Actor->IsA<APawn>()) {
		if (hitnormal.Z > 0.1f) {
			FVector dir = rot.Vector();
			dir = FVector::VectorPlaneProject(dir, hitnormal);
			rot = dir.Rotation();
			rot.Add(20.f, 0, 0);
		}
	}

	static FActorSpawnParameters spawnparams;
	spawnparams.Instigator = Instigator;
	spawnparams.Owner = Instigator;

	//DrawDebugLine(GetWorld(), hitlocation, hitlocation + rot.Vector() * 500.f, FColor::Red, false, 5.f, 0, 3.f);

	GetWorld()->SpawnActor<ARepulsor>(RepulsorClass, hitlocation, rot, spawnparams);
}
