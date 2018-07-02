#pragma once

#include "GameFramework/Actor.h"
#include "AbilityData.h"
#include "WeaponTypes.h"
#include "Effect.h"
#include "GoodGameCharacter.h"
#include "DataSingleton.h"
#include "Enum/StopCastingReason.h"
#include "Ability.generated.h"


UCLASS()
class GOODGAME_API AAbility : public AActor
{
	GENERATED_BODY()

public:
	const static int WeaponSlotsLengthBitMask = 1 << (int)EWeaponSlots::LENGTH;
	const static int PlayerCommonAbilities = 8;
	const static int PlayerMaxSchoolAbilities = 10;
	const static int PlayerAbilitySlots = PlayerCommonAbilities + PlayerMaxSchoolAbilities;

	AAbility();

	UPROPERTY(EditDefaultsOnly, Category = Ability)
	EAbilityType AbilityType;

	// No casting time required and spell cannot be held.
	UPROPERTY(EditDefaultsOnly, Category = Ability)
	bool bInstantCast;

	// Can this ability be user cancelled? Useful for things that have an animation delay like melee powers.
	UPROPERTY(EditDefaultsOnly, Category = Ability)
	bool bCanCancel = true;

	UFUNCTION(BlueprintCallable, Category = Ability)
	static bool CanCast(const AGoodGameCharacter* Character, const EAbilityType AbilityTypeID);

	UFUNCTION(BlueprintCallable, Category = Ability)
	static bool CantCastDueToWeaponSlot(const AGoodGameCharacter* Character, const FAbilityData& abilityData);

	/*UFUNCTION(BlueprintCallable, Category = Ability)
	static bool CanCastCached(const AGoodGameCharacter* Character, UPARAM(ref) const TArray<FAbilityData>& AbilityTypes, const EAbilityType AbilityTypeID);

	UFUNCTION(BlueprintCallable, Category = Ability)
	static bool CanCastByData(const AGoodGameCharacter* Character, UPARAM(ref) const FAbilityData& abilitydata);*/

	// Finish casting the ability.
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual void EndAbility(const EStopCastingReason Reason);

	// Removes the owner and deactives particle systems, sounds, etc. then sets the lifetime to 3 seconds to fade out but keeps the attachment.
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual void EndAbilityEffects();

	// Ability has started.
	UFUNCTION(BlueprintNativeEvent, Category = Ability)
	void OnAbilityStart();
	virtual void OnAbilityStart_Implementation();

	// Ability has stopped casting.
	UFUNCTION(BlueprintNativeEvent, Category = Ability)
	void OnAbilityEnd(const EStopCastingReason Reason);
	virtual void OnAbilityEnd_Implementation(const EStopCastingReason Reason);

	// If this ability has ended or not and is just sitting around to dissipate effects. Should not be used for game logic at all past this point.
	UFUNCTION(BlueprintCallable, Category = Ability)
	bool IsEnding() const { return EndingReason > 0; }

	// Absolute game time the spell can be released. Will always be 0 for instant cast.
	UFUNCTION(BlueprintCallable, Category = Ability)
	float GetFinishCastTime() const { return FinishCastTime; }

	// Get the amount of "progress" this ability should show on the HUD, if anything. A value of 0 means to not show any progress bar.
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual float GetHUDProgress() const;

	UFUNCTION(BlueprintCallable, Category = Ability)
	FAbilityData& GetAbilityData() const;

protected:
	UPROPERTY(Transient, VisibleAnywhere, ReplicatedUsing = OnRep_Ending, Category = Ability)
	uint8 EndingReason;

	UPROPERTY(Transient, VisibleAnywhere, Replicated, Category = Ability)
	float StartCastTime;

	UPROPERTY(Transient, VisibleAnywhere, Replicated, Category = Ability)
	float FinishCastTime;

	// DEPRECATED
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEffect> SuccessEffect;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* SuccessParticles;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	USoundCue* SuccessSound;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UPointLightComponent* LightComponent;

	// Cached pointer to GameSingleton
	UPROPERTY(Transient, BlueprintReadOnly, Category = Singleton)
	class UDataSingleton* GameSingleton;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	bool bSuccessEffectAttachToOwner = true;

	UFUNCTION()
	virtual void OnRep_Ending();

public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetOwner(AActor* NewOwner) override;

protected:
	virtual void OnRep_Owner() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
#endif

};
