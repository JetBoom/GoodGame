#ifndef __GOODGAME_H__
#define __GOODGAME_H__

#ifndef WORLD_TIME
#define WORLD_TIME (GetWorld()->GetGameState() ? GetWorld()->GetGameState()->GetServerWorldTimeSeconds() : 0.f)
#endif

#define MAX_PLAYER_BUILDS 20

#define ECC_AOE ECC_GameTraceChannel2

#include "Engine.h"

#include "GGStatics.h"

#include "Enum/StopCastingReason.h"

#include "DamageElement.h"
#include "DamageElementInfo.h"

#include "StatusEffectType.h"
#include "StatusEffectInfo.h"
#include "StatusEffectData.h"

#include "WeaponSlots.h"
#include "WeaponTypes.h"

#include "SchoolType.h"
#include "SchoolData.h"

#include "AbilityType.h"
#include "AbilityData.h"

#include "DataSingleton.h"

#endif
