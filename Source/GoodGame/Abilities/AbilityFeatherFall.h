#pragma once

#include "Ability.h"
#include "AbilityFeatherFall.generated.h"


UCLASS()
class GOODGAME_API AAbilityFeatherFall : public AAbility
{
	GENERATED_BODY()
	
protected:
	virtual void OnAbilityEnd_Implementation(const EStopCastingReason Reason) override;
	
};
