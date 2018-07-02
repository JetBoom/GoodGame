#pragma once

#include "Effect.h"
#include "RayEffect.generated.h"

UCLASS()
class GOODGAME_API ARayEffect : public AEffect
{
	GENERATED_BODY()

public:
	ARayEffect();

	UPROPERTY(ReplicatedUsing = UpdateParticleParameters, BlueprintReadWrite, Category = RayEffect)
	float RayDistance;

	UFUNCTION(BlueprintCallable, Category = RayEffect)
	virtual void UpdateParticleParameters();

	/*virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;*/

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RayEffect, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticleSystem;
	
};
