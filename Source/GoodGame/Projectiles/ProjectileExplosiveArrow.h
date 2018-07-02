#pragma once

#include "Projectile.h"
#include "ProjectileExplosiveArrow.generated.h"


UCLASS()
class GOODGAME_API AProjectileExplosiveArrow : public AProjectile
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	float OriginalDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ExplosiveArrow, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGoodGameDamageType> ProjectileDamageType2;
	
	virtual void BeginPlay() override;
	virtual void ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor) override;

};
