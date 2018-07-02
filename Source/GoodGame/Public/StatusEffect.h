#pragma once

#include "Components/SceneComponent.h"
#include "GoodGameCharacter.h"
#include "DamageElement.h"
#include "StatusEffect.generated.h"

USTRUCT(BlueprintType)
struct FDamageElementModifier
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Modifier)
	EDamageElement ElementID;

	UPROPERTY(EditAnywhere, Category = Modifier)
	float Modifier = 0.f;

	UPROPERTY(EditAnywhere, Category = Modifier)
	bool bMultiply = false;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom))
class GOODGAME_API AStatusEffect : public AActor
{
	GENERATED_BODY()

public:	
	AStatusEffect();

	virtual void BeginPlay() override;

	// Enum. Might be useful as a shortcut somewhere.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	EStatusEffectType StatusEffectID = EStatusEffectType::Generic;

	/* If true then there can only be one of these types of statuses on a player at once. Putting another one on will destroy the last one.
	* Should be true for things like buffs/debuffs and false for things that stack like damage over time. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bIsUnique = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	float AttackSpeedModifier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bAttackSpeedModifierIsMultiplier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	float BowSpeedModifier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bBowSpeedModifierIsMultiplier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	float CastingSpeedModifier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bCastingSpeedModifierIsMultiplier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	float MovementSpeedModifier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bMovementSpeedModifierIsMultiplier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	float CoolDownModifier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bCoolDownModifierIsMultiplier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	float TakeDamageModifier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bTakeDamageModifierIsMultiplier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	float GiveDamageModifier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bGiveDamageModifierIsMultiplier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	TArray<FDamageElementModifier> TakeDamageElementModifiers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	TArray<FDamageElementModifier> GiveDamageElementModifiers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	float StaminaUseModifier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bStaminaUseModifierIsMultiplier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	float ManaUseModifier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StatusEffect)
	bool bManaUseModifierIsMultiplier = false;

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	void RefreshAttachment();

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	void SetTimeRemaining(const float time);

	UFUNCTION(BlueprintCallable, Category = StatusEffect)
	virtual void SetNumberOfStacks(const uint8 NumStacks);

	// Used to apply additive modifiers.
	UFUNCTION(BlueprintNativeEvent, Category = StatusEffect)
	void ApplyModifiers(AGoodGameCharacter* Character);

	// Used to apply multiplicitive modifiers.
	UFUNCTION(BlueprintNativeEvent, Category = StatusEffect)
	void ApplyModifiersPassTwo(AGoodGameCharacter* Character);

	// Called right after damage is modified from resistances. Set bBlockDamage to block damage but still apply any impulse.
	UFUNCTION(BlueprintNativeEvent, Category = StatusEffect)
	void OnOwnerTakeDamage(UPARAM(ref) float& DamageAmount, UPARAM(ref) struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser, bool& bBlockDamage);

	// Called after damage calculations, impulse, etc. but right before health loss.
	UFUNCTION(BlueprintNativeEvent, Category = StatusEffect)
	void PostOnOwnerTakeDamage(UPARAM(ref) float& DamageAmount, UPARAM(ref) struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser);

	// Called right before health loss on someone else being damaged by our owner.
	UFUNCTION(BlueprintNativeEvent, Category = StatusEffect)
	void PostOnOwnerGiveDamage(const float DamageAmount, UPARAM(ref) struct FDamageEvent const& DamageEvent, const AGoodGameCharacter* Victim, const AActor* Inflictor);

public:
	// This is only for visuals on the client.
	UPROPERTY(ReplicatedUsing = OnRep_EndTime, BlueprintReadWrite, Category = StatusEffect)
	float EndTime;

	UPROPERTY(ReplicatedUsing = OnRep_NumberOfStacks, BlueprintReadWrite, Category = StatusEffect)
	uint8 NumberOfStacks;

	// This status shouldn't be included in attribute calculations. Marked as dead.
	UPROPERTY(BlueprintReadWrite, Category = StatusEffect)
	bool bInvalidStatus;

protected:
	UFUNCTION()
	virtual void OnRep_NumberOfStacks();

	UFUNCTION()
	virtual void OnRep_EndTime();

	virtual void OnRep_Owner() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
