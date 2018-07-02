

#pragma once

#include "GGStatics.generated.h"

UCLASS()
class GOODGAME_API UGGStatics : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	//UGGStatics(const FObjectInitializer& ObjectInitializer);

	// Shortcut to UGameplayStatics::SpawnSoundAttached on an actor's root component.
	static UAudioComponent* SpawnSoundOnActor(USoundCue* SoundCue, AActor* Actor);

	static bool IsVisible(const FVector& origin, const UPrimitiveComponent* component, const AActor* ignore = nullptr);

	static float InternalTakeRadialDamage(AActor* victim, float Damage, FRadialDamageEvent const& RadialDamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

};
