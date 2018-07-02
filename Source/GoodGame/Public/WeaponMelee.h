#pragma once

#include "Weapon.h"
#include "GoodGameDamageType.h"
#include "WeaponMelee.generated.h"


UCLASS()
class GOODGAME_API AWeaponMelee : public AWeapon
{
	GENERATED_BODY()
	
public:
	AWeaponMelee();

	// The base amount of damage the weapon does at 1.0 magnitude. Drastically changing magnitude of different weapon classes is not recommended since magnitude changes ABILITY power as well by the same amount.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeWeapon)
	float BaseDamage = 36.f;

	// The percent of the swing duration where hits will be processed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeWeapon)
	float SwingTimeHitFraction = 0.825f; //0.70f;

	// Percent of swing duration where sounds will be emitted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeWeapon)
	float SwingTimeSoundFraction = 0.5f;

	// How many seconds the ENTIRE swing will take to finish at base speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeWeapon)
	float BaseSwingTime = 0.9f; //1.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MeleeWeapon)
	USoundCue* SwingSound;

	// Damage type to apply to victims.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MeleeWeapon)
	TSubclassOf<UGoodGameDamageType> MeleeDamageType;

	// Total arc of the cone in degrees.
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeWeapon)
	float MeleeArc = 60.f;*/

	// Must be this much or above for dot product. Ensures a cone hit detection.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeWeapon)
	float MeleeMinDot = 0.5f;

	// How far the swing goes in cm.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeWeapon)
	float MeleeRange = 200.f;

	// Amount of stamina required to swing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeWeapon)
	float StaminaCost = 5.f;

	UPROPERTY(ReplicatedUsing = OnRep_HitProcessed, BlueprintReadOnly, Category = MeleeWeapon)
	uint32 bHitProcessed:1;

	UPROPERTY(ReplicatedUsing = OnRep_ShouldPlaySwingSound, BlueprintReadOnly, Category = MeleeWeapon)
	uint32 bShouldPlaySwingSound:1;

	UFUNCTION(BlueprintCallable, Category = MeleeWeapon)
	FORCEINLINE bool IsInSwing() const { return StartAttackTime > KINDA_SMALL_NUMBER && WORLD_TIME <= FinishAttackTime; }

	UFUNCTION(BlueprintCallable, Category = MeleeWeapon)
	void PlaySwingSound();

	UFUNCTION()
	virtual void OnRep_HitProcessed();

	UFUNCTION()
	virtual void OnRep_ShouldPlaySwingSound();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void ResetAttack() override;

	virtual void OnAttackPressed_Implementation() override;
	virtual void OnStartAttack_Implementation() override;
	
};
