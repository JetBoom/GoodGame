#pragma once

#include "GameFramework/Character.h"
#include "DataSingleton.h"
#include "HasHealth.h"
#include "HasTargetInfo.h"
#include "Projectile.h"
#include "Weapon.h"
#include "AbilityType.h"
#include "SchoolType.h"
#include "Ability.h"
#include "AbilityData.h"
#include "StatusEffect.h"
#include "GoodGameHUD.h"
#include "HealthComponent.h"
#include "StaminaManaComponent.h"
#include "GoodGameDamageType.h"
#include "Enum/StopCastingReason.h"
#include "CharacterMovementComponentEx.h"
#include "GoodGameCharacter.generated.h"


USTRUCT(BlueprintType)
struct FEquippedAbility
{
	GENERATED_USTRUCT_BODY()

	FEquippedAbility()
	{
		AbilityType = EAbilityType::LENGTH;
		NextAllowed = 0.f;
	}

	FEquippedAbility(const EAbilityType type)
	{
		AbilityType = type;
		NextAllowed = 0.f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EquippedAbility)
	EAbilityType AbilityType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EquippedAbility)
	float NextAllowed;
};


UENUM(BlueprintType)
enum class ECharacterCameraMode : uint8
{
	ThirdPerson = 0,
	OverRightShoulder,
	OverLeftShoulder,
	FirstPerson,
	MAX
};


UCLASS(config=Game)
class AGoodGameCharacter : public ACharacter, public IHasHealth, public IHasTargetInfo
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaminaMana, meta = (AllowPrivateAccess = "true"))
	class UStaminaManaComponent* StaminaManaComponent;

public:
	AGoodGameCharacter(const FObjectInitializer & ObjectInitializer);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stamina)
	float StaminaToJump = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stamina)
	float GlobalAbilityCoolDown = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Character)
	USoundCue* AggroSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Character)
	USoundCue* DeathSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Character)
	USoundCue* ReviveSound;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeapon, EditAnywhere, BlueprintReadWrite, Category = Weapon)
	class AWeapon* ActiveWeapon;

	// Ability actor we're casting. Note this is set inside the ability SetOwner / OnRep_Owner functions. No need to replicate this var because only one can be active at once.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	class AAbility* CastingAbility;

	// Currently selected ability slot. Sent to server when we do the key press.
	UPROPERTY(BlueprintReadWrite, Category = Ability)
	uint8 SelectedAbility;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedAbilities, EditAnywhere, BlueprintReadWrite, Category = Ability)
	TArray<FEquippedAbility> EquippedAbilities;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<AWeapon*> Weapons;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Ability)
	float GlobalCooldown;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	float AttackSpeedMultiplier = 1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	float CastingSpeedMultiplier = 1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	float BowSpeedMultiplier = 1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	float MovementSpeedMultiplier = 1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	float CoolDownMultiplier = 1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	float TakeDamageMultiplier = 1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	float GiveDamageMultiplier = 1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	float StaminaUseMultiplier = 1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	float ManaUseMultiplier = 1.f;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	TArray<float> TakeDamageMultiplierElement;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	TArray<float> GiveDamageMultiplierElement;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_AnyAttribute, BlueprintReadWrite, Category = Attributes)
	TArray<AStatusEffect*> StatusEffects;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Combat)
	uint32 bIsGuarding:1;
	
	UPROPERTY(BlueprintReadWrite, Category = Combat)
	float InCombatUntil;

	UFUNCTION(BlueprintCallable, Category = Combat)
	FORCEINLINE bool IsGuarding() const { return bIsGuarding; }

	UFUNCTION(BlueprintCallable, Category = Combat)
	virtual bool IsBusy() const;

	UFUNCTION(BlueprintCallable, Category = Combat)
	bool IsInCombat() const { return WORLD_TIME < InCombatUntil; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DamageEffects)
	USoundCue* BackDamageEffectSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DamageEffects)
	UParticleSystem* BackDamageEffectParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DamageEffects)
	USoundCue* GuardEffectSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = DamageEffects)
	UParticleSystem* GuardEffectParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	ECharacterCameraMode CharacterCameraMode = ECharacterCameraMode::ThirdPerson;

	UFUNCTION(BlueprintCallable, Category = Damage)
	bool CalculateGuard(float& DamageAmount, const FDamageEvent& DamageEvent);

	UFUNCTION(BlueprintCallable, Category = DamageEffects)
	void CreateDamageEffect(const uint8& DamageElementID, const bool& IsBackHit = false, const bool& IsGuard = false);

	UFUNCTION(BlueprintCallable, Category = Ability)
	void SetSelectedAbility(const uint8 AbilitySlot);

	UFUNCTION(BlueprintCallable, Category = Ability)
	float GetManaCost(UPARAM(ref) const FAbilityData& AbilityData) const { return AbilityData.ManaCost * (AbilityData.SchoolTypeID == ESchoolType::Common ? 1.f : ManaUseMultiplier); }

	UFUNCTION(BlueprintCallable, Category = Ability)
	float GetStaminaCost(UPARAM(ref) const FAbilityData& AbilityData) const { return AbilityData.StaminaCost * (AbilityData.SchoolTypeID == ESchoolType::Common ? 1.f : StaminaUseMultiplier); }

	UFUNCTION(BlueprintCallable, Category = Ability)
	bool AbilityIsOnCooldown(const FEquippedAbility& EquippedAbility) const { return WORLD_TIME <= EquippedAbility.NextAllowed; }

	UFUNCTION(BlueprintCallable, Category = Damage)
	void ModifyOutgoingDamage(float& Damage, const TSubclassOf<UGoodGameDamageType>& DmgType, const bool& UseWeaponMagnitude = true) const;

	UFUNCTION(BlueprintCallable, Category = StatusEffects)
	AStatusEffect* GetStatusEffect(const EStatusEffectType StatusEffectType);

	UFUNCTION(BlueprintCallable, Category = StatusEffects)
	bool HasStatusEffect(const EStatusEffectType StatusEffectType) const;

	// This needs to be called after any modifications are done to this component. The client calls it automatically.
	UFUNCTION()
	void AttributeCalculations();

	// Called from calculations, no need to call manually.
	UFUNCTION()
	void AttributeSanityChecks();

	UFUNCTION()
	void ResetAttributes();

	UFUNCTION()
	void RemoveStatus(AStatusEffect* Status, const bool SkipDestruction = false);

	UFUNCTION()
	AStatusEffect* AddStatusOfType(EStatusEffectType StatusType, float LifeTime = -1.f, bool ReplaceUnique = true);

	UFUNCTION()
	void AddStatus(AStatusEffect* Status);

	UFUNCTION()
	void InvalidateAttributeCalculations() { bShouldDoAttributeCalculations = true; }

	UFUNCTION(BlueprintCallable, Category = Health)
	bool IsDead() const;

public:
	// True when character pressing weapon button
	UPROPERTY(BlueprintReadOnly, Category = "Pawn|Character")
	uint32 bPressingUseWeapon:1;

	// True when character pressing ability button
	UPROPERTY(BlueprintReadOnly, Category = "Pawn|Character")
	uint32 bPressingUseAbility:1;

	// Cached pointer to HUD
	UPROPERTY(Transient, BlueprintReadOnly, Category = "HUD")
	AGoodGameHUD* HUD;

	// Cached pointer to GameSingleton
	UPROPERTY(Transient, BlueprintReadOnly, Category = Singleton)
	UDataSingleton* GameSingleton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	UCharacterMovementComponentEx* CharacterMovementEx;

	// Ignores all deploy/holster functions and immediately sets current weapon.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void SetActiveWeapon(AWeapon* Weapon);

	// Goes through the deploy/holster functions to change weapons.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual bool ChangeActiveWeapon(AWeapon* Weapon, bool force = false);

	// Checks if we have the specified weapon class.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool HasWeapon(const TSubclassOf<AWeapon> WeaponClass);

	// Give the specified weapon class to us. Returns a weapon pointer but can also return nullptr if unable to give. Can return a weapon already in our inventory if the type matches.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual AWeapon* GiveWeapon(const TSubclassOf<AWeapon> WeaponClass);

	// Get the specified weapon class in our weapon inventory. Can be nullptr.
	UFUNCTION(BlueprintCallable, Category = Weapon)
	AWeapon* GetWeapon(const TSubclassOf<AWeapon> WeaponClass);

	// Checks all conditions to see if we can cast this.
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual bool CanStartAbility(const EAbilityType AbilityTypeID);

	UFUNCTION(BlueprintNativeEvent, Category = Ability)
	void OnAbilityEnd(const EStopCastingReason Reason);

	// Used for changing HUD elements by event instead of tick.
	UFUNCTION(BlueprintCallable, Category = Ability)
	void EquippedAbilityChanged(const uint8& slot, const FEquippedAbility& oldEquippedAbility);

	// Calls CanStartAbility and returns the ability actor or nullptr if failed.
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual AAbility* StartAbility(const EAbilityType AbilityTypeID);

	UFUNCTION(BlueprintCallable, Category = Ability)
	FAbilityData GetAbilityData(const uint8& AbilitySlot) const;

	// Ends any ability we might be casting right now with a failure reason.
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual void EndAbility(const EStopCastingReason Reason = EStopCastingReason::Other);

	// Switch the equipped ability out in index <Slot> to the new AbilityType. Will take in to consideration swapping. If JustChecking then checks capability rather than actually doing it.
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual bool SwitchEquippedAbility(const uint8 Slot, const EAbilityType AbilityType, bool JustChecking = false);

	// Switch out the current build for the new one. New one must be completely valid and ALL current ones must be switchable. If JustChecking then checks capability rather than actually doing it.
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual bool SwitchEquippedAbilities(TArray<uint8> AbilityIDs, bool JustChecking = false);

	UFUNCTION(BlueprintCallable, Category = Camera)
	virtual void SwitchCameraMode(ECharacterCameraMode Mode);

	UFUNCTION(Server, Reliable, WithValidation, Category = GoodGameCharacter)
	void ServerPressWeaponButton();

	UFUNCTION(Server, Reliable, WithValidation, Category = GoodGameCharacter)
	void ServerReleaseWeaponButton();

	UFUNCTION(Server, Reliable, WithValidation, Category = GoodGameCharacter)
	void ServerPressAbilityButton(uint8 AbilitySlot);

	UFUNCTION(Server, Reliable, WithValidation, Category = GoodGameCharacter)
	void ServerReleaseAbilityButton();

	UFUNCTION(Server, Reliable, WithValidation, Category = GoodGameCharacter)
	void ServerPressCancelButton();

	UFUNCTION(Server, Reliable, WithValidation, Category = GoodGameCharacter)
	void ServerChangeActiveWeapon(uint8 WeaponIndex);

	UFUNCTION(Server, Reliable, WithValidation, Category = GoodGameCharacter)
	void ServerSwitchCameraMode(uint8 Mode);

	UFUNCTION(Server, Reliable, WithValidation, Category = Ability)
	void ServerSwitchEquippedAbility(uint8 Slot, uint8 AbilityTypeID);

	UFUNCTION(Server, Reliable, WithValidation, Category = Ability)
	void ServerSwitchEquippedAbilities(const TArray<uint8>& AbilityIDs);

	UFUNCTION(NetMulticast, Unreliable, Category = DamageEffects)
	void BroadcastDamageEffect(const uint8& DamageEffectData);

	// Set ability in slot <slot> to <abilityid>. Will fail if in combat, any skills are on cooldown, or the ability is not made yet. Slot is 1 - 10. See abilityids.txt for ability IDs
	UFUNCTION(Exec, Category = Ability)
	void SetAbility(uint8 slot, uint8 abilityid);

protected:
	/** Resets HMD orientation in VR. */
	//void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION()
	void OnRep_ActiveWeapon(AWeapon* OldWeapon);

	UFUNCTION()
	void OnRep_AnyAttribute();

	UFUNCTION()
	void OnRep_EquippedAbilities(TArray<FEquippedAbility> oldEquippedAbilities);

	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	void OnWeaponChanged(AWeapon* OldWeapon);

	// Marks the component to do a calculation on the next frame to save cycles.
	UPROPERTY(Transient)
	bool bShouldDoAttributeCalculations;

	//UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual float InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	template<uint8 WeaponIndex>
	void SelectWeaponSlot();

	template<const EAbilityType AbilityTypeID>
	void SelectAbilityByID();
	UFUNCTION(BlueprintCallable, Category = Ability)
	void SelectAbilityByID(const EAbilityType AbilityTypeID);

	template<uint8 AbilitySlot>
	void SelectAbilityBySlot();
	UFUNCTION(BlueprintCallable, Category = Ability)
	void SelectAbilityBySlot(const uint8 AbilitySlot);

	void LoadCameraMode();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual bool CanJumpInternal_Implementation() const override;

	virtual void CheckJumpInput(float DeltaTime) override;

	virtual void OnRep_Controller() override;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = Health)
	UHealthComponent* GetHealthComponent() const { return HealthComponent; };

	// IHasHealth
	virtual float GetHealth() const override { return HealthComponent->GetHealth(); };
	virtual float GetMaxHealth() const override { return HealthComponent->HealthMax; };
	virtual bool OnKilled(AActor* attacker = nullptr, AActor* inflictor = nullptr) override;
	virtual void OnRevived() override;
	// End IHasHealth

	// IHasTargetInfo
	virtual FString GetTargetName() const override;
	virtual FString GetTargetSubName() const override;
	// End IHasTargetInfo

	UFUNCTION(BlueprintNativeEvent, Category = PlayerInput)
	void OnPressUseWeapon();

	UFUNCTION(BlueprintNativeEvent, Category = PlayerInput)
	void OnReleaseUseWeapon();

	UFUNCTION(BlueprintNativeEvent, Category = PlayerInput)
	void OnPressUseAbility();

	UFUNCTION(BlueprintNativeEvent, Category = PlayerInput)
	void OnReleaseUseAbility();

	UFUNCTION(BlueprintNativeEvent, Category = PlayerInput)
	void OnPressNextAbility();

	UFUNCTION(BlueprintNativeEvent, Category = PlayerInput)
	void OnPressPreviousAbility();

	UFUNCTION(BlueprintNativeEvent, Category = PlayerInput)
	void OnPressCancel();

	UFUNCTION(BlueprintNativeEvent, Category = PlayerInput)
	void OnPressSwitchCameraMode();

	FORCEINLINE class UStaminaManaComponent* GetStaminaManaComponent() const { return StaminaManaComponent; }

	// Gets an adjusted pos/rotation for shooting things. Uses the camera pos and straight rotation for far away things or eye pos and adjusted rotation for close things.
	UFUNCTION(BlueprintCallable, Category = Camera)
	virtual void GetShootTransform(FVector& out_Location, FRotator& out_Rotation) const;

};
