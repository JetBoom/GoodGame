#pragma once

#include "GameFramework/Actor.h"
#include "WeaponSlots.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

UCLASS(BlueprintType)
class GOODGAME_API AWeapon : public AActor
{
	GENERATED_BODY()

	UAudioComponent* DeployHolsterAudio;
	
public:	
	AWeapon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	EWeaponSlots WeaponSlot = EWeaponSlots::Melee;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	EWeaponTypes WeaponType = EWeaponTypes::Sword2H;

	// Cached bitmask from our WeaponSlot, used in seeing if an ability can be used with us equipped.
	UPROPERTY(Transient, BlueprintReadWrite, Category = Weapon)
	int WeaponBitMask;

	// Magnitude of this weapon. Magnitude is used to calculate damage as well as ability magnitude, if the ability calls from weapon damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float WeaponMagnitude = 1.f;

	// All speed on weapons is based on a multiplier. The final calculated speed is done internally with character attributes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float WeaponSpeed = 1.f;

	// How much damage is multiplied by if we guard it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float WeaponGuardDamageMultiplier = 0.5f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_StartAttackTime, BlueprintReadWrite, Category = Weapon)
	float StartAttackTime;
	
	UPROPERTY(Transient, Replicated, BlueprintReadWrite, Category = Weapon)
	float FinishAttackTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	USoundCue* DeploySound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	USoundCue* HolsterSound;

	// Overrides the default character guard effect.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<AEffect> GuardEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	bool bShouldEverShowHUDProgress;

	// If this weapon is 'busy'. Disallows holstering, switching, attacking, casting, or doing pretty much anything besides finishing whatever we're busy with.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual bool IsBusy() const;

	// Attempt to holster this weapon and make it inactive. Checks if busy unless forced. Returns true on success.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool Holster(bool force = false);

	// Attempt to deploy this weapon and make it active. Checks if busy unless forced. Returns true on success.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void Deploy();

	// Called on server + all clients. Any kind of special logic on holster should be done here.
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	void OnHolster();

	// Called on server + all clients. Any kind of special logic on deploy should be done here.
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	void OnDeploy();

	// Primary attack button was pressed.
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	void OnAttackPressed();

	// Primary attack button was released.
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	void OnAttackReleased();

	// Cancel button was pressed.
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	void OnCancelPressed();

	// Called on server/client when attack has started. Can be used to dispatch effects on clients.
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	void OnStartAttack();

	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	void OnFinishAttack();

	// Called if we're the active weapon and the owner dies.
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	void OnOwnerKilled();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetAttackTimes(const float AttackStart, const float AttackEnd = 0.f);

	// Enable rendering the weapon.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void Show();

	// Disable rendering the weapon.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void Hide();

	// Reevaluate where we should be attached.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void RefreshAttachment();

	// Get the amount of "progress" this weapon should show on the HUD, if anything. A value of 0 means to not show any progress bar.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual float GetHUDProgress() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void ResetAttack();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	FORCEINLINE bool IsOwnerOrAuthority() { return Role == ROLE_Authority || GetOwner() && GetOwner()->Role >= ROLE_AutonomousProxy; }

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetOwner(AActor* NewOwner) override;

protected:
	UFUNCTION()
	virtual void OnRep_StartAttackTime(float OldStartAttackTime);
	
};
