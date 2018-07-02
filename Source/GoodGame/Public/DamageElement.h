

#pragma once

#include "DamageElement.generated.h"

UENUM(BlueprintType)
enum class EDamageElement : uint8
{
	Generic = 0,
	Slashing,
	Bashing,
	Piercing,
	Fire,
	Cold,
	Lightning,
	Acid,
	Divine,
	Curse,
	Almighty,
	Bleed,
	Poison,
	Impact,

	LENGTH
};
