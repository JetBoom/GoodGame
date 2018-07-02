#pragma once

#include "GameFramework/Actor.h"
#include "Effect.generated.h"


UCLASS(BlueprintType)
class GOODGAME_API AEffect : public AActor
{
	GENERATED_BODY()
	
public:
	AEffect();

	virtual void PostNetInit() override;

};
