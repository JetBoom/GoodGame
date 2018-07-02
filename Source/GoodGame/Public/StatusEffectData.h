

#pragma once

#include "StatusEffectType.h"
#include "StatusEffect.h"
#include "StatusEffectData.generated.h"

USTRUCT()
struct FStatusEffectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = StatusEffect)
	EStatusEffectType StatusEffectID = EStatusEffectType::Generic;

	UPROPERTY(EditAnywhere, Category = StatusEffect)
	FString Name = "Unknown";

	UPROPERTY(EditAnywhere, Category = StatusEffect)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, Category = StatusEffect)
	TSubclassOf<AStatusEffect> SceneComponentClass;

	UPROPERTY(EditAnywhere, Category = StatusEffect)
	bool bAllowedWhileDead = false;
};
