

#pragma once

#include "StatusEffectType.h"

#include "StatusEffectInfo.generated.h"

USTRUCT()
struct FStatusEffectInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = StatusEffect)
	EStatusEffectType StatusEffectID = EStatusEffectType::Generic;

	UPROPERTY(EditAnywhere, Category = StatusEffect)
	float EndTime = 0.f;
};
