#pragma once

UENUM(BlueprintType)
enum class EStopCastingReason : uint8
{
	None = 0,
	Success,
	NotEnoughStats,
	Cancel,
	Other
};
