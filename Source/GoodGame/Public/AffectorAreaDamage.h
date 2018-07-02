#pragma once

#include "AffectorArea.h"
#include "AffectorAreaDamage.generated.h"


UCLASS()
class GOODGAME_API AAffectorAreaDamage : public AAffectorArea
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AffectorAreaDamage)
	float Damage = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AffectorAreaDamage)
	TSubclassOf<UGoodGameDamageType> DamageType = UGoodGameDamageType::StaticClass();

protected:
	virtual void DoAffect(AActor* actor) override;
	virtual void UpdateFromProjectile(const AProjectile* projectile) override;

};
