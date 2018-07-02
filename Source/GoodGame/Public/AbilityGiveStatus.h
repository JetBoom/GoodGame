#pragma once

#include "Ability.h"
#include "StatusEffectType.h"
#include "AbilityGiveStatus.generated.h"

UCLASS()
class GOODGAME_API AAbilityGiveStatus : public AAbility
{
	GENERATED_BODY()

public:
	AAbilityGiveStatus();

	virtual void OnAbilityEnd_Implementation(const EStopCastingReason Reason) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"))
	EStatusEffectType StatusEffectType;
};
