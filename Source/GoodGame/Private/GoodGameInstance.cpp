#include "GoodGame.h"
#include "DataSingleton.h"
#include "GoodGameInstance.h"


void UGoodGameInstance::Init()
{
	Super::Init();

	auto gd = Cast<UDataSingleton>(GEngine->GameSingleton);

	check(gd && "Couldn't load from game singleton.");

	gd->LoadAbilityData();
	gd->LoadPlayerBuilds();
	gd->LoadCameraModes();
}
