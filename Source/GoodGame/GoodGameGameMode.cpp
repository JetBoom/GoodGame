#include "GoodGame.h"
#include "GoodGameGameMode.h"
#include "GoodGameCharacter.h"

AGoodGameGameMode::AGoodGameGameMode()
{
}

void AGoodGameGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	AGoodGameCharacter* character = Cast<AGoodGameCharacter>(PlayerPawn);
	if (character) {
		for (const auto& weaponclass : DefaultWeaponClasses)
			character->GiveWeapon(weaponclass);
	}
}
