#pragma once

#include "GameFramework/DamageType.h"
#include "DamageElement.h"
#include "GoodGameDamageType.generated.h"


UCLASS(const, Blueprintable, BlueprintType)
class GOODGAME_API UGoodGameDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	static bool CalculateBackDamage(float& DamageAmount, struct FDamageEvent const& DamageEvent, class AActor* const DamageReceiver);

	static FVector CalculateDamageImpulseOrigin(struct FDamageEvent const& DamageEvent, class AActor* const DamageReceiver, class AActor* const DamageCauser = nullptr);

	/* The basic element this damage is doing. Damage elements apply different visual hit effects and drain different amounts of mana / stamina.
	Protections and debuffs apply modifiers based on this. */
	UPROPERTY(EditDefaultsOnly, Category = DamageElement)
	EDamageElement DamageElement = EDamageElement::Generic;

	// Impulses caused by this damage type will be multiplied by this much against their own instigator.
	UPROPERTY(EditDefaultsOnly, Category = RigidBody)
	float SelfImpulseMultiplier = 0.5f;

	// Damage was dealt from a melee weapon. Used to signify being able to leech with melee life leech abilities.
	UPROPERTY(EditDefaultsOnly, Category = DamageType)
	bool bIsMelee = false;

	// Ignores player guarding.
	UPROPERTY(EditDefaultsOnly, Category = DamageType)
	bool bIgnoreGuard = false;

	// Will never deal back critical damage.
	UPROPERTY(EditDefaultsOnly, Category = DamageType)
	bool bNeverBackHit = false;

	// Describes how impulse magnitude falls off by Damage/MaxDamage. Higher values makes impulse magnitude fall off faster over distances. 0 ensures a full impulse magnitude no matter what damage.
	UPROPERTY(EditDefaultsOnly, Category = RigidBody)
	float DamageImpulseExp = 1.f;

	// This % of damage dealt will be returned to the owner as health.
	UPROPERTY(EditDefaultsOnly, Category = DamageType)
	float DamageLeechAmount = 0.f;

};
