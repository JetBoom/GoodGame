#pragma once

#include "GameFramework/GameModeBase.h"
#include "GoodGameGameMode.generated.h"

UCLASS(minimalapi)
class AGoodGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGoodGameGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	TArray<TSubclassOf<AWeapon>> DefaultWeaponClasses;

	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
};
