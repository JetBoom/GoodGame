

#pragma once

#include "EffectData.generated.h"

USTRUCT()
struct GOODGAME_API FEffectData
{
	GENERATED_USTRUCT_BODY();

public:
	FEffectData()
	{
	}

	FEffectData(const FVector_NetQuantize location, const FVector_NetQuantizeNormal normal, AActor* actor = nullptr)
	{
		Location = location;
		Normal = normal;
		Actor = actor;
	}

	UPROPERTY()
	FVector_NetQuantize Location = FVector_NetQuantize(0.f, 0.f, 0.f);

	UPROPERTY()
	FVector_NetQuantizeNormal Normal = FVector_NetQuantizeNormal(0.f, 0.f, 0.f);

	UPROPERTY()
	AActor* Actor = nullptr;
};
