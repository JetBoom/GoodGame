#pragma once

#include "StatusEffect.h"
#include "SEProtection.generated.h"


UCLASS()
class GOODGAME_API ASEProtection : public AStatusEffect
{
	GENERATED_BODY()
	
public:
	ASEProtection();

	UPROPERTY()
	bool bDamageBlocked;

public:
	virtual void ASEProtection::OnOwnerTakeDamage_Implementation(float& DamageAmount, struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser, bool& bBlockDamage) override;

};
