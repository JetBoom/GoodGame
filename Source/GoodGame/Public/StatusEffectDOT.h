#pragma once

#include "StatusEffect.h"
#include "GoodGameDamageType.h"
#include "StatusEffectDOT.generated.h"


UCLASS()
class GOODGAME_API AStatusEffectDOT : public AStatusEffect
{
	GENERATED_BODY()
	
public:
	AStatusEffectDOT();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DOT, meta = (AllowPrivateAccess = "true"))
	int Stacks = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DOT, meta = (AllowPrivateAccess = "true"))
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DOT, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGoodGameDamageType> DamageType;

	UFUNCTION()
	void DoDOT();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
};
