#pragma once

#include "StatusEffect.h"
#include "StatusEffectSOT.generated.h"


UCLASS()
class GOODGAME_API AStatusEffectSOT : public AStatusEffect
{
	GENERATED_BODY()

public:
	AStatusEffectSOT();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SOT, meta = (AllowPrivateAccess = "true"))
	int Stacks = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SOT, meta = (AllowPrivateAccess = "true"))
	float HealthPerStack = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SOT, meta = (AllowPrivateAccess = "true"))
	float StaminaPerStack = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SOT, meta = (AllowPrivateAccess = "true"))
	float ManaPerStack = 0.f;

	UFUNCTION()
	void DoSOT();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
