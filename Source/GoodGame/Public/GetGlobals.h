

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataSingleton.h"

#include "GetGlobals.generated.h"

UCLASS()
class GOODGAME_API UGetGlobals : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UGetGlobals(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "Game Data")
	static UDataSingleton* GetGameData(bool& IsValid);

};