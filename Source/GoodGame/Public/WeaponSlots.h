

#pragma once

#include "WeaponSlots.generated.h"

UENUM(BlueprintType)
enum class EWeaponSlots : uint8
{
	None = 1,
	Melee,
	Bow,
	Staff,
	Other,

	LENGTH
};
