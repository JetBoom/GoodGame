#pragma once

#include "StatusEffect.h"
#include "SEBarrage.generated.h"


UCLASS()
class GOODGAME_API ASEBarrage : public AStatusEffect
{
	GENERATED_BODY()
	
public:
	ASEBarrage();

	UPROPERTY(EditAnywhere, Category = Status)
	float LeechRatio = 0.2f;

public:
	virtual void PostOnOwnerGiveDamage_Implementation(const float DamageAmount, struct FDamageEvent const& DamageEvent, const AGoodGameCharacter* Victim, const AActor* Inflictor) override;
	
};
