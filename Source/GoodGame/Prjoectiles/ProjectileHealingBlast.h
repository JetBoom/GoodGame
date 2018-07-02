#pragma once

#include "Projectile.h"
#include "ProjectileHealingBlast.generated.h"


UCLASS()
class GOODGAME_API AProjectileHealingBlast : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileHealingBlast();

	virtual void BeginPlay() override;

protected:
	virtual void ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor = nullptr) override;
	
};
