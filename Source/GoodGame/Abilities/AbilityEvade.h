#pragma once

#include "Ability.h"
#include "AbilityEvade.generated.h"


UCLASS()
class GOODGAME_API AAbilityEvade : public AAbility
{
	GENERATED_BODY()

public:
	AAbilityEvade();

	UPROPERTY(EditAnywhere, Category = Evade)
	float ImpulseMagnitude = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category = Evade)
	float ImpulseAngle = 32.5f;

public:
	virtual void OnAbilityEnd_Implementation(const EStopCastingReason Reason) override;
	
};
