#pragma once

#include "Projectile.h"
#include "Repulsor.h"
#include "ProjectileBegone.generated.h"


UCLASS()
class GOODGAME_API AProjectileBegone : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileBegone();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Begone)
	TSubclassOf<ARepulsor> RepulsorClass;
	
	virtual void BeginPlay() override;
	virtual void ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor) override;

};
