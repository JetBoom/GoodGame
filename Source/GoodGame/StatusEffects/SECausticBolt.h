#pragma once

#include "StatusEffect.h"
#include "SECausticBolt.generated.h"


UCLASS()
class GOODGAME_API ASECausticBolt : public AStatusEffect
{
	GENERATED_BODY()

public:
	ASECausticBolt();

protected:
	virtual void SetNumberOfStacks(const uint8 NumStacks) override;
	
	
};
