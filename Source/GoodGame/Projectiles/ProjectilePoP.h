#pragma once

#include "Projectile.h"
#include "AffectorAreaDamage.h"
#include "ProjectilePoP.generated.h"


UCLASS()
class GOODGAME_API AProjectilePoP : public AProjectile
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Begone)
	TSubclassOf<AAffectorArea> AffectorClass;
	
	virtual void BeginPlay() override;
	virtual void ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor) override;
	
};
