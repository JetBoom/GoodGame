#pragma once

#include "StatusEffect.h"
#include "SEReviveSpell.generated.h"


UCLASS()
class GOODGAME_API ASEReviveSpell : public AStatusEffect
{
	GENERATED_BODY()
	
public:
	ASEReviveSpell();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};
