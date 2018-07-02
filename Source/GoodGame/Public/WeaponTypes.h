#pragma once

#include "WeaponTypes.generated.h"

// Might not need this.

UENUM(BlueprintType, meta = (BitFlags))
enum class EWeaponTypes : uint8
{
	None = 1,
	Sword2H,
	Axe2H,
	Hammer2H,
	Bow,
	Staff,
	Other,

	LENGTH
};
