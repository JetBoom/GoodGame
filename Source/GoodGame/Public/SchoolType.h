#pragma once

#include "SchoolType.generated.h"

UENUM(BlueprintType)
enum class ESchoolType : uint8
{
	Common = 0,
	Fire,
	Water,
	Earth,
	Air,
	Slayer,
	Knight,
	Berserker,
	Deadeye,
	Brawler,
	Duelist,
	Law,
	Chaos,
	Life,
	Pain,

	LENGTH
};
