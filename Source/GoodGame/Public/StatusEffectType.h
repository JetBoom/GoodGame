

#pragma once

#include "StatusEffectType.generated.h"

UENUM(BlueprintType)
enum class EStatusEffectType : uint8
{
	Generic = 0,
	Stampede,
	GodMode,
	Silence,
	GuardBreak, // TODO C++
	HealSelf,
	ManaToStamina,
	StaminaToHealth,
	HealthToMana,
	Bandage,
	Reinforce,
	SpellBane,
	HeightenedReflexes,
	Barrage,
	UncannyFocus,
	Protection,
	A2Fire,
	A2Earth,
	A2Air,
	A2Water,
	ExploitWeakness,
	Efficiency,
	PaleLeech, // NOTE: defunct, handled by damage volume
	Blizzard,
	ToxicRain,
	StaticField,
	VampiricStrike, // NOTE: Should set the healing power based on damage done.
	PowerAttackBleed,
	PowerAttackBlind, // TODO C++
	PowerAttackDebuff,
	CrushingBlow, // TODO C++ blind
	Lascerate,
	Pummel, // TODO C++
	Flurry,
	SoulFlay, // TODO C++
	ReviveSpell,
	Ganking, // TODO C++
	Reviving, // TODO C++
	Bastion,
	PrimalSurge,
	Invigoration,
	PungentMist,
	EyeRot,
	ManaLeech, // TODO C++
	CausticBolt, // NOTE: stacking handled by the projectile
	ManaShield,
	HuntersThorn,
	Frostbite,

	LENGTH
};
