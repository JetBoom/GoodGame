

#include "GoodGame.h"
#include "GetGlobals.h"

UGetGlobals::UGetGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UDataSingleton* UGetGlobals::GetGameData(bool& IsValid)
{
	IsValid = false;
	UDataSingleton* DataInstance = Cast<UDataSingleton>(GEngine->GameSingleton);

	if (!DataInstance) return NULL;
	if (!DataInstance->IsValidLowLevel()) return NULL;

	IsValid = true;
	return DataInstance;
}
