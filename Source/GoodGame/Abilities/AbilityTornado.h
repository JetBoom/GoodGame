#pragma once

#include "Ability.h"
#include "Repulsor.h"
#include "AbilityTornado.generated.h"


UCLASS()
class GOODGAME_API AAbilityTornado : public AAbility
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = Tornado)
	float TornadoDamage;

	UPROPERTY(EditDefaultsOnly, Category = Tornado)
	float TornadoRadius;

	UPROPERTY(EditDefaultsOnly, Category = Tornado)
	TSubclassOf<UDamageType> TornadoDamageType;

	UPROPERTY(EditDefaultsOnly, Category = Tornado)
	TSubclassOf<ARepulsor> TornadoRepulsor;

	virtual void OnAbilityEnd_Implementation(const EStopCastingReason Reason) override;

};
