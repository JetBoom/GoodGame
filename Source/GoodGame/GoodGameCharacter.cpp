#include "GoodGame.h"
#include "UnrealNetwork.h"
#include "GoodGamePlayerState.h"
//#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "GoodGameCharacter.h"


AGoodGameCharacter::AGoodGameCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponentEx>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.3f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	HealthComponent->HealthMax = HealthComponent->HealthBase = 450.f;
	HealthComponent->HealthRate = 1.f;

	StaminaManaComponent = CreateDefaultSubobject<UStaminaManaComponent>(TEXT("Stamina Mana"));
	StaminaManaComponent->StaminaMax = 400.f;
	StaminaManaComponent->StaminaRate = 2.5f;
	StaminaManaComponent->ManaMax = 400.f;
	StaminaManaComponent->ManaRate = 1.f;

	if (GEngine)
		GameSingleton = Cast<UDataSingleton>(GEngine->GameSingleton);

	static int numelements = (int)EDamageElement::LENGTH;
	if (TakeDamageMultiplierElement.Num() != numelements)
		TakeDamageMultiplierElement.SetNum(numelements, true);
	if (GiveDamageMultiplierElement.Num() != numelements)
		GiveDamageMultiplierElement.SetNum(numelements, true);
	for (int i = 0; i < numelements; ++i) {
		TakeDamageMultiplierElement[i] = 1.f;
		GiveDamageMultiplierElement[1] = 1.f;
	}

	if (EquippedAbilities.Num() != AAbility::PlayerAbilitySlots) {
		EquippedAbilities.SetNum(AAbility::PlayerAbilitySlots, true);
		for (int i = 0; i < AAbility::PlayerAbilitySlots; ++i)
			EquippedAbilities[i] = FEquippedAbility();
	}

	ResetAttributes();

	CharacterMovementEx = Cast<UCharacterMovementComponentEx>(GetCharacterMovement());
}

void AGoodGameCharacter::SetSelectedAbility(const uint8 AbilitySlot)
{
	const uint8 old = SelectedAbility;

	SelectedAbility = AbilitySlot;

	if (Controller && !IsNetMode(NM_DedicatedServer) && Controller->IsLocalController()) {
		//auto HUD = Cast<AGoodGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		if (HUD) {
			if (HUD->EquippedAbilityWidgets.Num() == AAbility::PlayerAbilitySlots) {
				HUD->EquippedAbilityWidgets[old]->OnEquippedAbilitySelectedChanged();
				HUD->EquippedAbilityWidgets[AbilitySlot]->OnEquippedAbilitySelectedChanged();
			}

			HUD->RefreshSelectedAbility();
		}

		// See if we can auto change to a weapon that CAN cast it.
		const auto ad = GetAbilityData(SelectedAbility);
		if (AAbility::CantCastDueToWeaponSlot(this, ad)) {
			for (AWeapon* weapon : Weapons) {
				if (weapon != ActiveWeapon && weapon) {
					if (ad.AllowedWeaponSlots == AAbility::WeaponSlotsLengthBitMask || ad.AllowedWeaponSlots > 0 && (ad.AllowedWeaponSlots & weapon->WeaponBitMask) != 0) {
						ChangeActiveWeapon(weapon);
						break;
					}
				}
			}
		}
	}
}

template<const uint8 WeaponIndex>
void AGoodGameCharacter::SelectWeaponSlot()
{
	if (WeaponIndex < Weapons.Num())
		ChangeActiveWeapon(Weapons[WeaponIndex]);
	else
		ChangeActiveWeapon(nullptr);
}

template<const EAbilityType AbilityTypeID>
void AGoodGameCharacter::SelectAbilityByID() { SelectAbilityByID(AbilityTypeID); }
void AGoodGameCharacter::SelectAbilityByID(const EAbilityType AbilityTypeID)
{
	if (AbilityTypeID != EAbilityType::LENGTH) {
		for (int i = 0; i < AAbility::PlayerAbilitySlots; ++i) {
			if (EquippedAbilities[i].AbilityType == AbilityTypeID) {
				SetSelectedAbility(i);
				break;
			}
		}
	}
}

template<uint8 AbilitySlot>
void AGoodGameCharacter::SelectAbilityBySlot() { SelectAbilityBySlot(AbilitySlot); }
void AGoodGameCharacter::SelectAbilityBySlot(const uint8 AbilitySlot)
{
	if (AbilitySlot < AAbility::PlayerAbilitySlots && EquippedAbilities[AbilitySlot].AbilityType != EAbilityType::LENGTH)
		SetSelectedAbility(AbilitySlot);
}

void AGoodGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	//check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoodGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoodGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGoodGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGoodGameCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AGoodGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AGoodGameCharacter::TouchStopped);

	// VR headset functionality
	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGoodGameCharacter::OnResetVR);

	PlayerInputComponent->BindAction("UseWeapon", IE_Pressed, this, &AGoodGameCharacter::OnPressUseWeapon);
	PlayerInputComponent->BindAction("UseWeapon", IE_Released, this, &AGoodGameCharacter::OnReleaseUseWeapon);

	PlayerInputComponent->BindAction("UseAbility", IE_Pressed, this, &AGoodGameCharacter::OnPressUseAbility);
	PlayerInputComponent->BindAction("UseAbility", IE_Released, this, &AGoodGameCharacter::OnReleaseUseAbility);

	PlayerInputComponent->BindAction("NextAbility", IE_Pressed, this, &AGoodGameCharacter::OnPressNextAbility);
	PlayerInputComponent->BindAction("PreviousAbility", IE_Pressed, this, &AGoodGameCharacter::OnPressPreviousAbility);

	PlayerInputComponent->BindAction("Cancel", IE_Pressed, this, &AGoodGameCharacter::OnPressCancel);

	PlayerInputComponent->BindAction("SwitchCameraMode", IE_Pressed, this, &AGoodGameCharacter::OnPressSwitchCameraMode);

	PlayerInputComponent->BindAction("WeaponSlot1", IE_Pressed, this, &AGoodGameCharacter::SelectWeaponSlot<0>);
	PlayerInputComponent->BindAction("WeaponSlot2", IE_Pressed, this, &AGoodGameCharacter::SelectWeaponSlot<1>);
	PlayerInputComponent->BindAction("WeaponSlot3", IE_Pressed, this, &AGoodGameCharacter::SelectWeaponSlot<2>);
	PlayerInputComponent->BindAction("WeaponSlot4", IE_Pressed, this, &AGoodGameCharacter::SelectWeaponSlot<3>);
	PlayerInputComponent->BindAction("WeaponSlot5", IE_Pressed, this, &AGoodGameCharacter::SelectWeaponSlot<4>);

	PlayerInputComponent->BindAction("SelectHealSelf", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityByID<EAbilityType::HealSelf>);
	PlayerInputComponent->BindAction("SelectStaminaToHealth", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityByID<EAbilityType::StaminaToHealth>);
	PlayerInputComponent->BindAction("SelectManaToStamina", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityByID<EAbilityType::ManaToStamina>);
	PlayerInputComponent->BindAction("SelectHealthToMana", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityByID<EAbilityType::HealthToMana>);
	PlayerInputComponent->BindAction("SelectEvade", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityByID<EAbilityType::Evade>);
	PlayerInputComponent->BindAction("SelectGuardBreak", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityByID<EAbilityType::GuardBreak>);
	PlayerInputComponent->BindAction("SelectBegone", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityByID<EAbilityType::Begone>);
	PlayerInputComponent->BindAction("SelectFeatherFall", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityByID<EAbilityType::FeatherFall>);
	PlayerInputComponent->BindAction("SelectSchoolAbility1", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities>);
	PlayerInputComponent->BindAction("SelectSchoolAbility2", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities + 1>);
	PlayerInputComponent->BindAction("SelectSchoolAbility3", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities + 2>);
	PlayerInputComponent->BindAction("SelectSchoolAbility4", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities + 3>);
	PlayerInputComponent->BindAction("SelectSchoolAbility5", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities + 4>);
	PlayerInputComponent->BindAction("SelectSchoolAbility6", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities + 5>);
	PlayerInputComponent->BindAction("SelectSchoolAbility7", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities + 6>);
	PlayerInputComponent->BindAction("SelectSchoolAbility8", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities + 7>);
	PlayerInputComponent->BindAction("SelectSchoolAbility9", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities + 8>);
	PlayerInputComponent->BindAction("SelectSchoolAbility10", IE_Pressed, this, &AGoodGameCharacter::SelectAbilityBySlot<AAbility::PlayerCommonAbilities + 9>);
}

/*void AGoodGameCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}*/

void AGoodGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AGoodGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AGoodGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGoodGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGoodGameCharacter::MoveForward(float Value)
{
	if (Value != 0.f && Controller) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AGoodGameCharacter::MoveRight(float Value)
{
	if (Value != 0.f && Controller) {
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AGoodGameCharacter::OnPressSwitchCameraMode_Implementation()
{
	switch (CharacterCameraMode)
	{
		case ECharacterCameraMode::ThirdPerson: {
			SwitchCameraMode(ECharacterCameraMode::OverRightShoulder);
			break;
		}
		case ECharacterCameraMode::OverRightShoulder: {
			SwitchCameraMode(ECharacterCameraMode::OverLeftShoulder);
			break;
		}
		case ECharacterCameraMode::OverLeftShoulder: {
			SwitchCameraMode(ECharacterCameraMode::FirstPerson);
			break;
		}
		case ECharacterCameraMode::FirstPerson: {
			SwitchCameraMode(ECharacterCameraMode::ThirdPerson);
			break;
		}
	}

	if (!HasAuthority())
		ServerSwitchCameraMode((uint8)CharacterCameraMode);

	if (IsLocallyControlled()) {
		uint8 slot = 0;
		if (ActiveWeapon)
			slot = (uint8)ActiveWeapon->WeaponSlot;

		// Bow and staff shares same camera mode.
		if (ActiveWeapon && (ActiveWeapon->WeaponSlot == EWeaponSlots::Bow || ActiveWeapon->WeaponSlot == EWeaponSlots::Staff)) {
			GameSingleton->CameraModeForWeapon[(int)EWeaponSlots::Bow] = GameSingleton->CameraModeForWeapon[(int)EWeaponSlots::Staff] = (uint8)CharacterCameraMode;
			GameSingleton->SaveCameraMode((int)EWeaponSlots::Bow);
			GameSingleton->SaveCameraMode((int)EWeaponSlots::Staff);
		}
		else {
			GameSingleton->CameraModeForWeapon[slot] = (uint8)CharacterCameraMode;
			GameSingleton->SaveCameraMode(slot);
		}
	}
}

void AGoodGameCharacter::SwitchCameraMode(ECharacterCameraMode Mode)
{
	CharacterCameraMode = Mode;

	static FName headname("head");

	if (Mode == ECharacterCameraMode::FirstPerson) {
		if (IsLocallyControlled())
			GetMesh()->HideBoneByName(headname, EPhysBodyOp::PBO_None);

		CameraBoom->SocketOffset = FVector::ZeroVector;
		CameraBoom->TargetArmLength = 0.f;
	}
	else {
		if (IsLocallyControlled())
			GetMesh()->UnHideBoneByName(headname);

		if (Mode == ECharacterCameraMode::ThirdPerson) {
			CameraBoom->SocketOffset = FVector::ZeroVector;
			CameraBoom->TargetArmLength = 400.f;
		}
		else {
			CameraBoom->TargetArmLength = 175.f;
			CameraBoom->SocketOffset = FVector(0, 65.f * (Mode == ECharacterCameraMode::OverRightShoulder ? 1.f : -1.f), 0);
		}
	}
}

void AGoodGameCharacter::OnPressNextAbility_Implementation()
{
	uint8 slot = SelectedAbility;

	uint8 tries = 0;
	do
	{
		++slot;
		if (slot >= AAbility::PlayerAbilitySlots)
			slot = 0;
	} while (tries < AAbility::PlayerAbilitySlots && EquippedAbilities[slot].AbilityType == EAbilityType::LENGTH);

	SetSelectedAbility(slot);
}

void AGoodGameCharacter::OnPressPreviousAbility_Implementation()
{
	uint8 slot = SelectedAbility;

	uint8 tries = 0;
	do
	{
		if (slot == 0)
			slot = AAbility::PlayerAbilitySlots - 1;
		else
			--slot;
	} while (tries < AAbility::PlayerAbilitySlots && EquippedAbilities[slot].AbilityType == EAbilityType::LENGTH);

	SetSelectedAbility(slot);
}

void AGoodGameCharacter::OnPressUseWeapon_Implementation()
{
	if (!HasAuthority())
		ServerPressWeaponButton();

	if (bPressingUseWeapon)
		return;

	bPressingUseWeapon = true;

	if (ActiveWeapon)
		ActiveWeapon->OnAttackPressed();
}

void AGoodGameCharacter::OnPressCancel_Implementation()
{
	if (!HasAuthority())
		ServerPressCancelButton();

	if (CastingAbility /*&& HasAuthority()*/ && CastingAbility->bCanCancel)
		CastingAbility->EndAbility(EStopCastingReason::Cancel);

	if (ActiveWeapon)
		ActiveWeapon->OnCancelPressed();
}

void AGoodGameCharacter::OnReleaseUseWeapon_Implementation()
{
	if (!HasAuthority())
		ServerReleaseWeaponButton();

	if (!bPressingUseWeapon)
		return;

	bPressingUseWeapon = false;

	if (ActiveWeapon)
		ActiveWeapon->OnAttackReleased();
}

void AGoodGameCharacter::OnPressUseAbility_Implementation()
{
	/*if (SelectedAbility >= AAbility::PlayerAbilitySlots)
		return;*/

	if (!HasAuthority())
		ServerPressAbilityButton(SelectedAbility);

	if (bPressingUseAbility)
		return;

	bPressingUseAbility = true;

	StartAbility(EquippedAbilities[SelectedAbility].AbilityType);
}

void AGoodGameCharacter::OnReleaseUseAbility_Implementation()
{
	if (!HasAuthority())
		ServerReleaseAbilityButton();

	if (!bPressingUseAbility)
		return;

	bPressingUseAbility = false;
}

void AGoodGameCharacter::ResetAttributes()
{
	MovementSpeedMultiplier =
		AttackSpeedMultiplier =
		BowSpeedMultiplier =
		CastingSpeedMultiplier =
		CoolDownMultiplier =
		TakeDamageMultiplier =
		GiveDamageMultiplier =
		StaminaUseMultiplier =
		ManaUseMultiplier = 1.f;

	for (int i = 0; i < (uint8)EDamageElement::LENGTH; ++i) {
		TakeDamageMultiplierElement[i] =
			GiveDamageMultiplierElement[i] = 1.f;
	}
}

bool AGoodGameCharacter::CanStartAbility(const EAbilityType AbilityTypeID)
{
	if (CastingAbility || HealthComponent->IsDead())
		return false;

	const float curtime = WORLD_TIME;

	// GCD
	if (curtime < GlobalCooldown)
		return false;

	for (const auto& equipped : EquippedAbilities) {
		if (equipped.AbilityType == AbilityTypeID)
			return curtime >= equipped.NextAllowed && AAbility::CanCast(this, AbilityTypeID);
	}

	// Ability not equipped.
	return false;
}

AAbility* AGoodGameCharacter::StartAbility(const EAbilityType AbilityTypeID)
{
	if (!HasAuthority() || !CanStartAbility(AbilityTypeID))
		return nullptr;

	const FAbilityData& abilitydata = GameSingleton->AbilityTypes[(int)AbilityTypeID];

	// So players can't spam start+cancel over and over for tons of effects.
	if (abilitydata.CastingTime > 0.f)
		GlobalCooldown = FMath::Max(GlobalCooldown, WORLD_TIME + 0.15f);

	if (abilitydata.ActorClass) {
		FActorSpawnParameters spawnparams;
		//spawnparams.Instigator = this;
		spawnparams.Owner = this;

		return GetWorld()->SpawnActor<AAbility>(abilitydata.ActorClass, GetActorLocation(), GetActorRotation(), spawnparams);
	}

	return nullptr;
}

void AGoodGameCharacter::EndAbility(const EStopCastingReason Reason)
{
	if (CastingAbility && HasAuthority() && !CastingAbility->IsEnding())
		CastingAbility->EndAbility(Reason);
}

void AGoodGameCharacter::OnAbilityEnd_Implementation(const EStopCastingReason Reason)
{
	if (!CastingAbility)
		return;

	if (Reason == EStopCastingReason::Success) {
		const float curtime = WORLD_TIME;

		if (GlobalAbilityCoolDown > 0.f)
			GlobalCooldown = FMath::Max(GlobalCooldown, curtime + GlobalAbilityCoolDown);

		static FAbilityData data;
		int slot = -1;

		for (int i = 0; i < EquippedAbilities.Num(); ++i) {
			if (EquippedAbilities[i].AbilityType == CastingAbility->AbilityType) {
				data = GameSingleton->AbilityTypes[(int)EquippedAbilities[i].AbilityType];
				slot = i;
				break;
			}
		}

		if (slot < 0)
			return;

		const float multiplier = data.SchoolTypeID == ESchoolType::Common ? 1.f : CoolDownMultiplier;
		const float cooldown = curtime + data.CoolDown * multiplier;

		FEquippedAbility old = EquippedAbilities[slot];
		EquippedAbilities[slot].NextAllowed = cooldown; // curtime + data.CoolDown * multiplier;
		EquippedAbilityChanged(slot, old);

		if (data.SharedCooldowns.Num() > 0) {
			for (int i = 0; i < EquippedAbilities.Num(); ++i) {
				const FAbilityData& otherData = GameSingleton->AbilityTypes[(int)EquippedAbilities[i].AbilityType];
				if (otherData.SharedCooldowns.Contains(data.AbilityTypeID) && cooldown < EquippedAbilities[i].NextAllowed) {
					old = EquippedAbilities[i];
					EquippedAbilities[i].NextAllowed = cooldown; // curtime + FMath::Min(data.CoolDown, otherData.CoolDown) * multiplier;
					EquippedAbilityChanged(i, old);
				}
			}
		}
	}
}

void AGoodGameCharacter::SetAbility(uint8 slot, uint8 type)
{
	if (slot < 1 || slot > AAbility::PlayerMaxSchoolAbilities || type >= (uint8)EAbilityType::LENGTH)
		return;

	slot = slot + AAbility::PlayerCommonAbilities - 1;

	if (HasAuthority())
		SwitchEquippedAbility(slot, (EAbilityType)type, false);
	else
		ServerSwitchEquippedAbility(slot, type);
}

void AGoodGameCharacter::ServerSwitchCameraMode_Implementation(uint8 Mode) { SwitchCameraMode((ECharacterCameraMode)Mode); }
bool AGoodGameCharacter::ServerSwitchCameraMode_Validate(uint8 Mode) { return Mode >= 0 && Mode < (uint8)ECharacterCameraMode::MAX; }

void AGoodGameCharacter::ServerSwitchEquippedAbility_Implementation(uint8 Slot, uint8 AbilityTypeID) {
	if (AbilityTypeID != 255 && AbilityTypeID < (uint8)EAbilityType::LENGTH)
		SwitchEquippedAbility(Slot, (EAbilityType)AbilityTypeID, false);
}
bool AGoodGameCharacter::ServerSwitchEquippedAbility_Validate(uint8 Slot, uint8 AbilityTypeID) { return Slot < AAbility::PlayerAbilitySlots && (AbilityTypeID < (uint8)EAbilityType::LENGTH || AbilityTypeID == 255); }

void AGoodGameCharacter::ServerSwitchEquippedAbilities_Implementation(const TArray<uint8>& AbilityIDs)
{
	SwitchEquippedAbilities(AbilityIDs, false);
}
bool AGoodGameCharacter::ServerSwitchEquippedAbilities_Validate(const TArray<uint8>& AbilityIDs) { return true; }

void AGoodGameCharacter::ServerPressCancelButton_Implementation() { OnPressCancel(); }
bool AGoodGameCharacter::ServerPressCancelButton_Validate() { return true; }

void AGoodGameCharacter::ServerChangeActiveWeapon_Implementation(uint8 WeaponIndex)
{
	if (WeaponIndex == 255)
		ChangeActiveWeapon(nullptr);
	else if (WeaponIndex < Weapons.Num())
		ChangeActiveWeapon(Weapons[WeaponIndex]);
}
bool AGoodGameCharacter::ServerChangeActiveWeapon_Validate(uint8 WeaponIndex) { return true; }

void AGoodGameCharacter::ServerPressWeaponButton_Implementation() { OnPressUseWeapon(); }
bool AGoodGameCharacter::ServerPressWeaponButton_Validate() { return true; }

void AGoodGameCharacter::ServerReleaseWeaponButton_Implementation() { OnReleaseUseWeapon(); }
bool AGoodGameCharacter::ServerReleaseWeaponButton_Validate() { return true; }

void AGoodGameCharacter::ServerPressAbilityButton_Implementation(uint8 AbilitySlot)
{
	SetSelectedAbility(AbilitySlot);
	OnPressUseAbility();
}
bool AGoodGameCharacter::ServerPressAbilityButton_Validate(uint8 AbilitySlot) { return AbilitySlot < AAbility::PlayerAbilitySlots; }

void AGoodGameCharacter::ServerReleaseAbilityButton_Implementation() { OnReleaseUseAbility(); }
bool AGoodGameCharacter::ServerReleaseAbilityButton_Validate() { return true; }

float AGoodGameCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	const float maxDamageForImpulse = DamageAmount;

	// This converts falloff damage to direct damage.
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// Only server handles actual damage processing.
	if (!HasAuthority())
		return DamageAmount;

	const UGoodGameDamageType* DmgType = Cast<UGoodGameDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());

	uint8 DamageElementID = 0;
	bool isBackHit = false;
	bool isGuard = false;
	bool blockDamage = false;

	if (DmgType)
	{
		DamageElementID = (uint8)DmgType->DamageElement;

		InCombatUntil = WORLD_TIME + 10.f;

		// Take in to consideration our resistances. We handle multipliers for the attacker in the attacks themselves.
		DamageAmount *= TakeDamageMultiplier;
		DamageAmount *= TakeDamageMultiplierElement[DamageElementID];

		for (auto& status : StatusEffects)
			status->OnOwnerTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser, blockDamage);

		if (DamageAmount <= 0.f)
			return 0.f;

		if (!DmgType->bCausedByWorld) {
			isBackHit = !DmgType->bNeverBackHit && UGoodGameDamageType::CalculateBackDamage(DamageAmount, DamageEvent, this);

			if (!isBackHit && !DmgType->bIgnoreGuard)
				isGuard = CalculateGuard(DamageAmount, DamageEvent);
		}

		// Drain % of damage as mana and stamina if the damage element calls for it.
		if (StaminaManaComponent)
		{
			const FDamageElementInfo& elementinfo = GameSingleton->DamageElementTypes[DamageElementID];

			if (elementinfo.StaminaDrainAmount > 0.f)
				StaminaManaComponent->TakeStaminaRaw(DamageAmount * elementinfo.StaminaDrainAmount);

			if (elementinfo.ManaDrainAmount > 0.f)
				StaminaManaComponent->TakeManaRaw(DamageAmount * elementinfo.ManaDrainAmount);
		}

		// Apply a force if the damage type calls for it.
		if (DmgType->DamageImpulse != 0.f) {
			FVector impulsedir = GetActorLocation() - UGoodGameDamageType::CalculateDamageImpulseOrigin(DamageEvent, this, DamageCauser);
			float impulsemag = DmgType->DamageImpulse;

			// Skew force towards horizontal movement.
			impulsedir.Z *= 0.666f;
			impulsedir.Normalize();

			// Scale impulse by damage/maxdamage
			if (DmgType->DamageImpulseExp > 0.f)
				impulsemag *= FMath::Pow(FMath::Min(DamageAmount / maxDamageForImpulse, 1.f), DmgType->DamageImpulseExp);

			// If applying a downward force, reduce the Z magnitude to stop air combos applying a lot of fall damage.
			if (impulsedir.Z < 0.f)
				impulsemag *= 0.5f;

			// Damage vs. ourselves applies a smaller force to prevent using explosive spells for bunny hopping.
			if (DmgType->SelfImpulseMultiplier && DamageCauser && DamageCauser->Instigator == this)
				impulsemag *= DmgType->SelfImpulseMultiplier;

			// Dead people can't be pushed around too much.
			if (HealthComponent->IsDead())
				impulsemag *= 0.5f;

			GetCharacterMovement()->AddImpulse(impulsedir * impulsemag, true);
		}
	}

	if (!IsDead()) {
		// Dispatch a damage effect based on the element
		CreateDamageEffect(DamageElementID, isBackHit, isGuard);

		for (auto& status : StatusEffects)
			status->PostOnOwnerTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

		// Owner of the inflictor should be a pawn or character.
		AActor* pawnOwner = DamageCauser;
		if (pawnOwner && !pawnOwner->IsA<APawn>())
			pawnOwner = pawnOwner->GetOwner();

		// Take the health. IHasHealth has an optional OnKilled.
		if (!blockDamage && HealthComponent) {
			AGoodGameCharacter* characterAttacker = Cast<AGoodGameCharacter>(pawnOwner);
			if (characterAttacker) {
				for (auto& status : characterAttacker->StatusEffects)
					status->PostOnOwnerGiveDamage(DamageAmount, DamageEvent, this, DamageCauser);

				if (characterAttacker != this) {
					AGoodGamePlayerState* attackerState = Cast<AGoodGamePlayerState>(characterAttacker->PlayerState);
					if (attackerState)
						attackerState->SendDamageNumber(this, DamageAmount, isBackHit);
				}
			}

			AGoodGamePlayerState* victimState = Cast<AGoodGamePlayerState>(PlayerState);
			if (victimState)
				victimState->SendDamageNumber(this, DamageAmount, isBackHit);

			HealthComponent->TakeHealth(DamageAmount, pawnOwner, DamageCauser);

			if (DmgType && DmgType->DamageLeechAmount != 0.f && characterAttacker)
				characterAttacker->GetHealthComponent()->GiveHealth(DamageAmount * DmgType->DamageLeechAmount);
		}
	}

	return DamageAmount;
}

void AGoodGameCharacter::ModifyOutgoingDamage(float& Damage, const TSubclassOf<UGoodGameDamageType>& DmgType, const bool& UseWeaponMagnitude) const
{
	Damage *= GiveDamageMultiplier;
	if (DmgType)
		Damage *= GiveDamageMultiplierElement[(int)DmgType->GetDefaultObject<UGoodGameDamageType>()->DamageElement];
	if (UseWeaponMagnitude && ActiveWeapon)
		Damage *= ActiveWeapon->WeaponMagnitude;
}

float AGoodGameCharacter::InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	return UGGStatics::InternalTakeRadialDamage(this, Damage, RadialDamageEvent, EventInstigator, DamageCauser);
}

bool AGoodGameCharacter::CalculateGuard(float& DamageAmount, const FDamageEvent& DamageEvent)
{
	if (ActiveWeapon && DamageEvent.IsOfType(FPointDamageEvent::ClassID) && IsGuarding() && ActiveWeapon->WeaponGuardDamageMultiplier != 1.f) {
		auto pde = (const FPointDamageEvent&)DamageEvent;

		FVector impactNorm = pde.HitInfo.ImpactNormal;
		impactNorm.Z = 0.f;
		impactNorm.Normalize();

		FVector actorNorm = GetActorForwardVector();
		actorNorm.Z = 0.f;
		actorNorm.Normalize();

		if (FVector::DotProduct(impactNorm, actorNorm) > 0.1f /*0.333f*/) {
			DamageAmount *= ActiveWeapon->WeaponGuardDamageMultiplier;
			return true;
		}
	}

	return false;
}

bool AGoodGameCharacter::OnKilled(AActor* attacker, AActor* inflictor)
{
	if (HasAuthority()) {
		StaminaManaComponent->ManaRate = 0.f;
		StaminaManaComponent->SetMana(0);
		StaminaManaComponent->StaminaRate = 0.f;
		StaminaManaComponent->SetStamina(0);

		for (auto& status : StatusEffects) {
			status->SetLifeSpan(0.0001f);
			status->bInvalidStatus = true;
		}

		InvalidateAttributeCalculations();

		// Give the killer some stats back.
		const AGoodGameCharacter* characterAttacker = Cast<AGoodGameCharacter>(attacker);
		if (characterAttacker) {
			characterAttacker->GetHealthComponent()->GiveHealth(25.f);
			characterAttacker->GetStaminaManaComponent()->GiveMana(25.f);
			characterAttacker->GetStaminaManaComponent()->GiveStamina(25.f);
		}
	}

	// Shrink the capsule so it doesn't block.
	/*GetCapsuleComponent()->SetRelativeLocation(FVector(0.f, 0.f, -50.f), false, nullptr, ETeleportType::TeleportPhysics);
	GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleRadius());*/

	//GetMesh()->SetRelativeLocation(FVector(32.f, 0.f, 0.f), false);

	if (DeathSound && !IsNetMode(NM_DedicatedServer))
		UGGStatics::SpawnSoundOnActor(DeathSound, this);

	if (ActiveWeapon)
		ActiveWeapon->OnOwnerKilled();

	return true;
}

void AGoodGameCharacter::OnRevived()
{
	if (HasAuthority()) {
		HealthComponent->SetHealth(10.f);

		StaminaManaComponent->StaminaRate = 2.5f;
		StaminaManaComponent->ManaRate = 1.f;
		StaminaManaComponent->SetMana(30.f);
		StaminaManaComponent->SetStamina(30.f);

		InvalidateAttributeCalculations();
	}

	// Put the capsule back.
	/*GetCapsuleComponent()->SetRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);*/

	//GetMesh()->SetRelativeLocation(FVector::ZeroVector, false);

	if (ReviveSound && !IsNetMode(NM_DedicatedServer))
		UGGStatics::SpawnSoundOnActor(ReviveSound, this);
}

FString AGoodGameCharacter::GetTargetName() const
{
	if (PlayerState)
		return PlayerState->PlayerName;

	const static FString empty;
	return empty;
}

FString AGoodGameCharacter::GetTargetSubName() const
{
	/*if (PlayerState)
		return PlayerState->ClanName;*/

	const static FString empty;
	return empty;
}

void AGoodGameCharacter::SetActiveWeapon(AWeapon* Weapon)
{
	if (!HasAuthority() || Weapon && Weapon->GetOwner() != this || Weapon == ActiveWeapon)
		return;

	AWeapon* OldActiveWeapon = ActiveWeapon;

	ActiveWeapon = Weapon;

	if (ActiveWeapon != OldActiveWeapon)
		OnWeaponChanged(OldActiveWeapon);
}

bool AGoodGameCharacter::ChangeActiveWeapon(AWeapon* Weapon, bool force)
{
	// Only server or local player can do this.
	if (Role < ROLE_AutonomousProxy)
		return false;

	// Let server know we want to change weapons. Send a uint8 of the weapon slot.
	if (!HasAuthority()) {
		if (Weapon) {
			for (int i = 0; i < Weapons.Num(); ++i) {
				if (Weapons[i] == Weapon)
					ServerChangeActiveWeapon(i);
			}
		}
		else
			ServerChangeActiveWeapon(255); // 255 = no weapon

		// Client is done here. Might need to be changed later for smoother transitions.
		return false;
	}

	// Nothing to do here.
	if (CastingAbility || IsDead() || Weapon && Weapon->GetOwner() != this || ActiveWeapon == Weapon)
		return false;

	// If we can't holster our current weapon then don't bother trying to deploy the new one.
	if (ActiveWeapon && !ActiveWeapon->Holster(force))
		return false;

	if (Weapon)
		Weapon->Deploy();

	return true;
}

bool AGoodGameCharacter::HasWeapon(const TSubclassOf<AWeapon> WeaponClass)
{
	return GetWeapon(WeaponClass) != nullptr;
}

AWeapon* AGoodGameCharacter::GetWeapon(const TSubclassOf<AWeapon> WeaponClass)
{
	for (auto &Weapon : Weapons) {
		if (Weapon && Weapon->IsA(WeaponClass) && Weapon->GetOwner() == this)
			return Weapon;
	}

	return nullptr;
}

AWeapon* AGoodGameCharacter::GiveWeapon(const TSubclassOf<AWeapon> WeaponClass)
{
	if (!HasAuthority())
		return nullptr;

	AWeapon* NewWeapon = GetWeapon(WeaponClass);

	if (!NewWeapon) {
		FActorSpawnParameters spawnparams;
		spawnparams.Owner = this;

		NewWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorLocation(), GetActorRotation(), spawnparams);
	}

	return NewWeapon;
}

void AGoodGameCharacter::OnWeaponChanged_Implementation(AWeapon* OldWeapon)
{
	if (OldWeapon)
		OldWeapon->OnHolster();

	if (ActiveWeapon)
		ActiveWeapon->OnDeploy();

	if (IsLocallyControlled())
		LoadCameraMode();
}

void AGoodGameCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (IsLocallyControlled())
		LoadCameraMode();
}

void AGoodGameCharacter::LoadCameraMode()
{
	uint8 slot = 0;

	if (ActiveWeapon)
		slot = (uint8)ActiveWeapon->WeaponSlot;

	if (GameSingleton && slot < GameSingleton->CameraModeForWeapon.Num()) {
		const auto newmode = (ECharacterCameraMode)GameSingleton->CameraModeForWeapon[slot];

		if (CharacterCameraMode != newmode) {
			SwitchCameraMode(newmode);
			if (!HasAuthority())
				ServerSwitchCameraMode((uint8)CharacterCameraMode);
		}
	}
}

void AGoodGameCharacter::OnRep_ActiveWeapon(AWeapon* OldWeapon)
{
	OnWeaponChanged(OldWeapon);
}

void AGoodGameCharacter::OnRep_AnyAttribute()
{
	// If any replicated variable changes then we mark the component to do the calculations next frame.
	InvalidateAttributeCalculations();
}

void AGoodGameCharacter::CreateDamageEffect(const uint8& DamageElementID, const bool& IsBackHit, const bool& IsGuard)
{
	if (HasAuthority()) {
		uint8 effectData = DamageElementID & 15;
		if (IsBackHit)
			effectData |= 128;
		if (IsGuard)
			effectData |= 64;

		BroadcastDamageEffect(effectData);
	}

	if (IsNetMode(NM_DedicatedServer))
		return;

	const FDamageElementInfo& elementdata = GameSingleton->DamageElementTypes[DamageElementID];

	if (elementdata.DamageEffectSound)
		UGGStatics::SpawnSoundOnActor(elementdata.DamageEffectSound, this);

	if (elementdata.DamageEffectParticles)
		UGameplayStatics::SpawnEmitterAttached(elementdata.DamageEffectParticles, RootComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);

	if (IsBackHit) {
		if (BackDamageEffectSound)
			UGGStatics::SpawnSoundOnActor(BackDamageEffectSound, this);
		if (BackDamageEffectParticles)
			UGameplayStatics::SpawnEmitterAttached(BackDamageEffectParticles, RootComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}

	if (IsGuard) {
		if (GuardEffectSound)
			UGGStatics::SpawnSoundOnActor(BackDamageEffectSound, this);
		if (GuardEffectParticles)
			UGameplayStatics::SpawnEmitterAttached(BackDamageEffectParticles, RootComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}
}

void AGoodGameCharacter::BroadcastDamageEffect_Implementation(const uint8& DamageEffectData)
{
	if (HasAuthority())
		return;

	// First bit is used to signify back damage or not
	const bool isBackHit = (DamageEffectData & 128) == 128;
	// Second bit is used to signify a guard or not
	const bool isGuard = (DamageEffectData & 64) == 64;
	// Bits 3-4 reserved for future use...
	// Last 4 bits are used to signify a damage element ID (16 of them is enough).
	const uint8 DamageElementID = DamageEffectData & 15;

	CreateDamageEffect(DamageElementID, isBackHit, isGuard);

	InCombatUntil = WORLD_TIME + 10.f;
}

void AGoodGameCharacter::AttributeCalculations()
{
	bShouldDoAttributeCalculations = false;

	if (!HasAuthority() && !IsLocallyControlled())
		return;

	// Reset everything to base stats, temporarily.
	ResetAttributes();

	// Let all the status effects do their modifications on the attributes.
	for (auto& status : StatusEffects) {
		if (status && !status->bInvalidStatus && !status->IsActorBeingDestroyed())
			status->ApplyModifiers(this);
	}
	for (auto& status : StatusEffects) {
		if (status && !status->bInvalidStatus && !status->IsActorBeingDestroyed())
			status->ApplyModifiersPassTwo(this);
	}

	// Lastly, make sure the attributes are in valid ranges.
	AttributeSanityChecks();

	// Handle movement speed stat
	UCharacterMovementComponentEx* Movement = (UCharacterMovementComponentEx*)GetCharacterMovement();

	Movement->SetMaxWalkSpeedMultiplier(HealthComponent->IsDead() ? 0.f : MovementSpeedMultiplier);
	//Movement-> // TODO: Leap
}

void AGoodGameCharacter::RemoveStatus(AStatusEffect* Status, const bool SkipDestruction)
{
	if (StatusEffects.RemoveSingle(Status) > 0) {
		Status->bInvalidStatus = true;
		if (!SkipDestruction)
			Status->Destroy();

		InvalidateAttributeCalculations();
	}
}

void AGoodGameCharacter::AttributeSanityChecks()
{
	MovementSpeedMultiplier = FMath::Clamp(MovementSpeedMultiplier, 0.f, 5.f);
	AttackSpeedMultiplier = FMath::Clamp(AttackSpeedMultiplier, 0.2f, 3.f);
	BowSpeedMultiplier = FMath::Clamp(BowSpeedMultiplier, 0.2f, 3.f);
	CastingSpeedMultiplier = FMath::Clamp(CastingSpeedMultiplier, 0.2f, 3.f);
	CoolDownMultiplier = FMath::Clamp(CoolDownMultiplier, 0.f, 2.f);

	StaminaUseMultiplier = FMath::Max(StaminaUseMultiplier, 0.f);
	ManaUseMultiplier = FMath::Max(ManaUseMultiplier, 0.f);
	
	// Can remove this in the future to make people absorb damage elements but for now should be 0 or above.
	TakeDamageMultiplier = FMath::Max(TakeDamageMultiplier, 0.f);
	GiveDamageMultiplier = FMath::Max(TakeDamageMultiplier, 0.f);
	for (int i = 0; i < (int)EDamageElement::LENGTH; ++i) {
		TakeDamageMultiplierElement[i] = FMath::Max(TakeDamageMultiplierElement[i], 0.f);
		GiveDamageMultiplierElement[i] = FMath::Max(GiveDamageMultiplierElement[i], 0.f);
	}
}

AStatusEffect* AGoodGameCharacter::GetStatusEffect(const EStatusEffectType StatusEffectType)
{
	for (auto& status : StatusEffects) {
		if (status && status->StatusEffectID == StatusEffectType && !status->bInvalidStatus)
			return status;
	}

	return nullptr;
}

bool AGoodGameCharacter::HasStatusEffect(const EStatusEffectType StatusEffectType) const
{
	for (auto& status : StatusEffects) {
		if (status && status->StatusEffectID == StatusEffectType && !status->bInvalidStatus)
			return true;
	}

	return false;
}

AStatusEffect* AGoodGameCharacter::AddStatusOfType(EStatusEffectType StatusType, float lifetime, bool ReplaceUnique)
{
	// Sanity check. A NEW status shouldn't be added on the client since the actor itself is replicated.
	if (!HasAuthority())
		return nullptr;

	AStatusEffect* newstatus = nullptr;

	// If we already have a status of this type and it's marked as unique, it needs to be replaced.
	for (auto& status : StatusEffects) {
		if (status && status->StatusEffectID == StatusType && !status->bInvalidStatus) {
			if (!status->bIsUnique)
				break;

			if (ReplaceUnique) {
				status->SetLifeSpan(0.001f); // Remove it next frame.
				status->bInvalidStatus = true;
			}
			else {
				newstatus = status;
				break;
			}
		}
	}

	const FStatusEffectData& statusdata = GameSingleton->StatusEffectTypes[(int)StatusType];

	if (HealthComponent->IsDead() && !statusdata.bAllowedWhileDead)
		return nullptr;

	if (newstatus) {
		newstatus->SetTimeRemaining(lifetime);
		return newstatus;
	}

	FActorSpawnParameters spawnparams;
	spawnparams.Owner = this;

	newstatus = GetWorld()->SpawnActor<AStatusEffect>(statusdata.SceneComponentClass, GetActorLocation(), GetActorRotation(), spawnparams);
	if (newstatus) {
		AddStatus(newstatus);
		newstatus->SetTimeRemaining(lifetime);
	}

	return newstatus;
}

void AGoodGameCharacter::AddStatus(AStatusEffect* Status)
{
	if (!StatusEffects.Contains(Status)) {
		Status->SetOwner(this);
		Status->RefreshAttachment();
		StatusEffects.Push(Status);

		InvalidateAttributeCalculations();
	}
}

bool AGoodGameCharacter::IsDead() const
{
	return HealthComponent->IsDead();
}

void AGoodGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<AGoodGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	GameSingleton = Cast<UDataSingleton>(GEngine->GameSingleton);

	// "Equip" all common abilities no matter what.
	//EquippedAbilities.Empty();
	uint8 AbilitySlot = 0;
	for (const auto& abilitydata : GameSingleton->AbilityTypes) {
		if (abilitydata.SchoolTypeID == ESchoolType::Common)
			EquippedAbilities[AbilitySlot++].AbilityType = abilitydata.AbilityTypeID;
	}

	if (HasAuthority()) {
		SwitchCameraMode(ECharacterCameraMode::ThirdPerson);

		// Test abilities
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::Stampede;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::FireBall;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::ChargedBolt;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::CausticBolt;
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::TrueShot;
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::ExhaltedSacrifice;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::Invigoration;
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::ExplosiveShot;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::Impale;
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::BitterBlast;
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::HealingBlast;
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::BattleRevive;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::DragonBreath;
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::AcidRock;
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::MagmaBomb;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::Barrage;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::Efficiency;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::HeightenedReflexes;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::UncannyFocus;
		//EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::Lunge;
		EquippedAbilities[AbilitySlot++].AbilityType = EAbilityType::Flurry;
	}

	if (IsLocallyControlled())
		LoadCameraMode();
}

void AGoodGameCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority()) {
		for (AWeapon* weapon : Weapons) {
			if (weapon)
				weapon->SetLifeSpan(KINDA_SMALL_NUMBER);
		}
	}
}

void AGoodGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldDoAttributeCalculations)
		AttributeCalculations();
}

bool AGoodGameCharacter::IsBusy() const
{
	return CastingAbility || IsDead() || IsGuarding() || ActiveWeapon && ActiveWeapon->IsBusy();
}

bool AGoodGameCharacter::SwitchEquippedAbility(const uint8 Slot, const EAbilityType AbilityType, bool JustChecking)
{
	// Only server can actually execute the switching.
	if (!HasAuthority())
		JustChecking = true;

	const auto& abilitydata = GameSingleton->AbilityTypes[(int)AbilityType];

	// Can never equip a common ability because every single one is always equipped and LOCKED in to a slot.
	if (abilitydata.SchoolTypeID == ESchoolType::Common)
		return false;

	// Ability not implemented.
	if (abilitydata.Description.Equals(FString("Description")))
		return false;

	// Can't change ability that's on cooldown.
	if (Slot >= AAbility::PlayerAbilitySlots || Slot < AAbility::PlayerCommonAbilities || AbilityIsOnCooldown(EquippedAbilities[Slot]))
		return false;

	// Can't be doing any of this stuff.
	if (IsBusy() || StatusEffects.Num() > 0 || IsInCombat())
		return false;

	// New ability must have an entry in data table.
	if ((uint8)AbilityType >= GameSingleton->AbilityTypes.Num())
		return false;

	uint8 i;
	bool isswap = false;

	// If we already have this ability equipped, then we're merely switching slots.
	for (i = 0; i < AAbility::PlayerAbilitySlots; ++i) {
		if (EquippedAbilities[i].AbilityType == AbilityType) {
			// But we need to check cooldowns on that slot as well and can't swap a common slot.
			if (GameSingleton->AbilityTypes[(int)EquippedAbilities[i].AbilityType].SchoolTypeID == ESchoolType::Common || AbilityIsOnCooldown(EquippedAbilities[i]))
				return false;

			if (!JustChecking) {
				FEquippedAbility old = EquippedAbilities[i];

				EquippedAbilities[i].AbilityType = EquippedAbilities[Slot].AbilityType;
				EquippedAbilities[i].NextAllowed = EquippedAbilities[Slot].NextAllowed;

				EquippedAbilityChanged(i, old);
			}

			isswap = true;

			break;
		}
	}

	// If we're NOT doing a swap, then we need to make sure we wouldn't have more than allowed school abilities equipped at once.
	/*if (!isswap) {
		uint8 schoolabilities = 0;
		for (i = 0; i < AAbility::PlayerAbilitySlots; ++i) {
			if (GameSingleton->AbilityTypes[(int)EquippedAbilities[i].AbilityType].SchoolTypeID != ESchoolType::Common) {
				++schoolabilities;
				if (schoolabilities >= AAbility::PlayerMaxSchoolAbilities)
					return false;
			}
		}
	}*/

	if (!JustChecking) {
		FEquippedAbility old = EquippedAbilities[Slot];
		
		EquippedAbilities[Slot].AbilityType = AbilityType;
		EquippedAbilities[Slot].NextAllowed = WORLD_TIME + 3.f;// FMath::Max(abilitydata.CoolDown, 3.f);

		EquippedAbilityChanged(Slot, old);
	}

	return true;
}

bool AGoodGameCharacter::SwitchEquippedAbilities(TArray<uint8> AbilityIDs, bool JustChecking)
{
	// Only server can actually execute the switching.
	if (!HasAuthority())
		JustChecking = true;

	// Can't be doing any of this stuff.
	if (IsBusy() || StatusEffects.Num() > 0 || IsInCombat())
		return false;

	// Can't switch if any non-common ability is on cooldown.
	for (const auto& equipped : EquippedAbilities) {
		if (GameSingleton->AbilityTypes[(int)equipped.AbilityType].SchoolTypeID != ESchoolType::Common && AbilityIsOnCooldown(equipped))
			return false;
	}

	// Ensure num = number of school abilities.
	const int num = AbilityIDs.Num();
	if (num != AAbility::PlayerMaxSchoolAbilities)
		return false;

	// Ensure only one copy of every ability.
	TSet<uint8> has;
	for (int i = 0; i < AAbility::PlayerMaxSchoolAbilities; ++i) {
		if (has.Contains(AbilityIDs[i]))
			return false;

		has.Add(AbilityIDs[i]);
	}

	const auto maxAbilityIndex = GameSingleton->AbilityTypes.Num() - 1;

	// Ensure not trying to add invalid or common abilities.
	for (int i = 0; i < AAbility::PlayerMaxSchoolAbilities; ++i) {
		if (AbilityIDs[i] > maxAbilityIndex)
			return false;

		const auto& abilitydata = GameSingleton->AbilityTypes[AbilityIDs[i]];

		if (abilitydata.SchoolTypeID == ESchoolType::Common)
			return false;
	}

	if (!JustChecking) {
		TArray<FEquippedAbility> old = EquippedAbilities;

		const auto cooldown = WORLD_TIME + 3.f;
		for (int i = 0; i < AAbility::PlayerMaxSchoolAbilities; ++i) {
			EquippedAbilities[i].AbilityType = (EAbilityType)AbilityIDs[i];
			EquippedAbilities[i].NextAllowed = cooldown;
		}

		OnRep_EquippedAbilities(old);
	}

	return true;
}

FAbilityData AGoodGameCharacter::GetAbilityData(const uint8& AbilitySlot) const
{
	if (EquippedAbilities[AbilitySlot].AbilityType == EAbilityType::LENGTH)
		return GameSingleton->AbilityTypes[0];

	return GameSingleton->AbilityTypes[(int)EquippedAbilities[AbilitySlot].AbilityType];
}

void AGoodGameCharacter::OnRep_EquippedAbilities(TArray<FEquippedAbility> oldEquippedAbilities)
{
	if (Controller && !IsNetMode(NM_DedicatedServer) && Controller->IsLocalController()) {
		for (int i = 0; i < AAbility::PlayerAbilitySlots; ++i)
			EquippedAbilityChanged(i, oldEquippedAbilities[i]);
	}
}

void AGoodGameCharacter::EquippedAbilityChanged(const uint8& slot, const FEquippedAbility& oldEquippedAbility)
{
	if (HUD) {
		if (EquippedAbilities[slot].AbilityType != oldEquippedAbility.AbilityType)
			HUD->EquippedAbilityWidgets[slot]->SetAbilityIndex(slot); // Fetches character abilities and implies a RefreshCooldown.
		else if (EquippedAbilities[slot].NextAllowed != oldEquippedAbility.NextAllowed)
			HUD->EquippedAbilityWidgets[slot]->RefreshCooldown();

		if (SelectedAbility == slot)
			HUD->RefreshSelectedAbility();
	}
}

void AGoodGameCharacter::GetShootTransform(FVector& out_Location, FRotator& out_Rotation) const
{
	if (CharacterCameraMode != ECharacterCameraMode::OverRightShoulder && CharacterCameraMode != ECharacterCameraMode::OverLeftShoulder/* || InFreeCam()*/) {
		out_Location = GetPawnViewLocation();
		out_Rotation = GetBaseAimRotation();
		return;
	}

	const FVector eyeLocation = GetPawnViewLocation();
	FRotator eyeRot = GetBaseAimRotation();
	FVector offset = CameraBoom->SocketOffset;
	offset.X = 0.f;
	const float checkLength = offset.Size() * 10.f;

	// If we're remote client then aim up/down based on RemoteViewPitch byte.
	if (FMath::IsNearlyZero(eyeRot.Pitch))
		eyeRot.Pitch = RemoteViewPitch * 360.f / 255.f;

	const FVector cameraLocation = eyeLocation + eyeRot.Quaternion() * offset;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, eyeLocation, cameraLocation + eyeRot.Vector() * checkLength, ECC_Visibility, FCollisionQueryParams(NAME_None, false, this))) {
		// Shoot from eye. Rotate angle.
		out_Location = eyeLocation;
		out_Rotation = FMath::RInterpConstantTo(eyeRot, (Hit.Location - eyeLocation).ToOrientationRotator(), 1.f, 45.f);
	}
	else {
		// Shoot from camera. Don't rotate angle.
		out_Location = cameraLocation;
		out_Rotation = eyeRot;
	}
}

bool AGoodGameCharacter::CanJumpInternal_Implementation() const
{
	return !HealthComponent->IsDead() && (!StaminaManaComponent || StaminaManaComponent->HasStamina(StaminaToJump)) && Super::CanJumpInternal_Implementation();
}

void AGoodGameCharacter::CheckJumpInput(float DeltaTime)
{
	if (CharacterMovementEx)
	{
		if (bPressedJump)
		{
			// Increment our timer first so calls to IsJumpProvidingForce() will return true
			JumpKeyHoldTime += DeltaTime;

			if (CharacterMovementEx->IsFalling())
			{
				if (JumpCurrentCount == 0)
					JumpCurrentCount++;

				if (CharacterMovementEx->JumpInAirTime < KINDA_SMALL_NUMBER)
					CharacterMovementEx->JumpInAirTime = GetWorld()->TimeSeconds;
			}

			const bool bDidJump = CanJump() && CharacterMovementEx->DoJump(bClientUpdating);
			if (!bWasJumping && bDidJump)
			{
				JumpCurrentCount++;
				OnJumped();
			}

			bWasJumping = bDidJump;
		}

		// If the jump key is no longer pressed and the character is no longer falling,
		// but it still "looks" like the character was jumping, reset the counters.
		else if (bWasJumping && !CharacterMovementEx->IsFalling())
		{
			ResetJumpState();
		}
	}
}

void AGoodGameCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGoodGameCharacter, bIsGuarding);
	DOREPLIFETIME(AGoodGameCharacter, ActiveWeapon);
	DOREPLIFETIME(AGoodGameCharacter, StatusEffects);

	DOREPLIFETIME_CONDITION(AGoodGameCharacter, Weapons, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGoodGameCharacter, EquippedAbilities, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGoodGameCharacter, GlobalCooldown, COND_OwnerOnly);

	DOREPLIFETIME(AGoodGameCharacter, AttackSpeedMultiplier);
	DOREPLIFETIME(AGoodGameCharacter, BowSpeedMultiplier);
	DOREPLIFETIME_CONDITION(AGoodGameCharacter, CastingSpeedMultiplier, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGoodGameCharacter, MovementSpeedMultiplier, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGoodGameCharacter, TakeDamageMultiplier, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGoodGameCharacter, GiveDamageMultiplier, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGoodGameCharacter, TakeDamageMultiplierElement, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGoodGameCharacter, GiveDamageMultiplierElement, COND_OwnerOnly);
}
