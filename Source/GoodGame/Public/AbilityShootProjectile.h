#pragma once

#include "Ability.h"
#include "Projectile.h"
#include "AbilityShootProjectile.generated.h"


UCLASS()
class GOODGAME_API AAbilityShootProjectile : public AAbility
{
	GENERATED_BODY()
	
public:
	AAbilityShootProjectile();

	virtual void OnAbilityEnd_Implementation(const EStopCastingReason Reason) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectile> ProjectileClass;
	
};
