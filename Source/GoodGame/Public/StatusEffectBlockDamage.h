#pragma once

#include "StatusEffect.h"
#include "StatusEffectBlockDamage.generated.h"


UCLASS()
class GOODGAME_API AStatusEffectBlockDamage : public AStatusEffect
{
	GENERATED_BODY()
	
public:
	virtual void OnOwnerTakeDamage_Implementation(float& DamageAmount, struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser, bool& bBlockDamage) override;

};
