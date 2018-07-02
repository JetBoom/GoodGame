#pragma once

#include "Projectile.h"
#include "ProjectileCausticBolt.generated.h"


UCLASS()
class GOODGAME_API AProjectileCausticBolt : public AProjectile
{
	GENERATED_BODY()
	
protected:
	virtual void ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor) override;
	
};
