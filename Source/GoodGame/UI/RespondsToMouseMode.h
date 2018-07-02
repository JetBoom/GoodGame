#pragma once

#include "RespondsToMouseMode.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URespondsToMouseMode : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class GOODGAME_API IRespondsToMouseMode
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HUD)
	void OnMouseModeSet(const bool mouseEnabled);
	
};
