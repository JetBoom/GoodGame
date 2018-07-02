

#pragma once

#include "Effect.h"
#include "DamageElementInfo.generated.h"

USTRUCT()
struct FDamageElementInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Damage)
	EDamageElement ElementID = EDamageElement::Generic;

	UPROPERTY(EditAnywhere, Category = Damage)
	FString Name = "Unknown";

	UPROPERTY(EditAnywhere, Category = Damage)
	float StaminaDrainAmount = 0.15f;

	UPROPERTY(EditAnywhere, Category = Damage)
	float ManaDrainAmount = 0.f;

	UPROPERTY(EditAnywhere, Category = Damage)
	UParticleSystem* DamageEffectParticles;

	UPROPERTY(EditAnywhere, Category = Damage)
	USoundCue* DamageEffectSound;
};
