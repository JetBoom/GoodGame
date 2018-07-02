#pragma once

#include "GoodGame.h"
#include "AbilityType.h"
#include "SchoolType.h"
#include "WeaponSlots.h"
#include "Ability.h"
#include "AbilityData.generated.h"

class AAbility;

USTRUCT(BlueprintType)
struct FAbilityData
{
	GENERATED_USTRUCT_BODY()

	FAbilityData()
	{
		AbilityTypeID = EAbilityType::LENGTH;
	}

	// Don't worry about setting this. Auto filled on game load.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (MakeStructureDefaultValue = "EAbilityType::LENGTH"))
	EAbilityType AbilityTypeID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	ESchoolType SchoolTypeID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	TSubclassOf<AAbility> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	FString Name = "Name";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	FString Description = "Description";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	float ManaCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	float StaminaCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	float HealthCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	float CastingTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	float CoolDown = 10.f;

	// Use 0 for always cast or use LENGTH for any weapon.
	UPROPERTY(EditAnywhere, Category = Ability, meta=(Bitmask, BitmaskEnum = EWeaponSlots))
	uint32 AllowedWeaponSlots;

	// Is cast speed scaled at all? Should be off for common abilities.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	bool bSpeedScaled = true;

	// Is cast speed scaled by the speed of the weapon? Should be on for most abilities.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	bool bSpeedScaledByWeapon = true;

	// Is cast speed scaled by the melee speed character attribute? Should be on for melee attacks.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	bool bSpeedScaledByAttackSpeed = false;

	// Is cast speed scaled by the casting speed character attribute? Should be on for magic attacks.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	bool bSpeedScaledByCastingSpeed = true;

	// Caster must be on the ground.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	bool bMustBeOnGround = false;

	// Caster must be standing still.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	bool bMustBeStationary = false;

	// Caster cannot be swimming.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	bool bMustBeDry = false;

	// Is cast speed scaled by the bow speed character attribute? Should be on for bow abilities.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	bool bSpeedScaledByBowSpeed = false;

	// Casting this ability, or any ability in the following list, will put ALL grouped abilities on cooldown for THIS ability's cooldown duration.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
	TArray<EAbilityType> SharedCooldowns;

};
