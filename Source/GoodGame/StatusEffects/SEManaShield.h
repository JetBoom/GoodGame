#pragma once

#include "StatusEffect.h"
#include "SEManaShield.generated.h"


UCLASS()
class GOODGAME_API ASEManaShield : public AStatusEffect
{
	GENERATED_BODY()
	
public:
	ASEManaShield();

public:
	virtual void OnOwnerTakeDamage_Implementation(float& DamageAmount, struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser, bool& bBlockDamage) override;
	
};
