#pragma once

#include "HasHealth.generated.h"

UINTERFACE(MinimalAPI)
class UHasHealth : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


class GOODGAME_API IHasHealth
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(/*BlueprintNativeEvent, BlueprintCallable, Category = Health*/)
	virtual float GetHealth() const;

	UFUNCTION(/*BlueprintNativeEvent, BlueprintCallable, Category = Health*/)
	virtual float GetMaxHealth() const;

	// Return false to indicate that the actor resisted death.
	UFUNCTION(/*BlueprintNativeEvent, BlueprintCallable, Category = Health*/)
	virtual bool OnKilled(AActor* attacker = nullptr, AActor* inflictor = nullptr);

	UFUNCTION(/*BlueprintNativeEvent, BlueprintCallable, Category = Health*/)
	virtual void OnRevived();

};
