#include "GoodGame.h"
#include "UnrealNetwork.h"
#include "HealthComponent.h"
#include "StaminaManaComponent.h"
#include "Weapon.h"
#include "GoodGameCharacter.h"
#include "SchoolType.h"
#include "AbilityData.h"
#include "Ability.h"


AAbility::AAbility()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	LightComponent->SetupAttachment(RootComponent);
	LightComponent->Intensity = 0.f; //6000.f;
	LightComponent->AttenuationRadius = 300.f;
	LightComponent->bAffectTranslucentLighting = false;

	SetActorEnableCollision(false);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	NetPriority = 1.f;
	NetUpdateFrequency = 20.f;
	SetReplicates(true);
	
	bCanBeDamaged = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (GEngine)
		GameSingleton = Cast<UDataSingleton>(GEngine->GameSingleton);
}

FAbilityData& AAbility::GetAbilityData() const
{
	return GameSingleton->AbilityTypes[(uint8)AbilityType];
}

bool AAbility::CantCastDueToWeaponSlot(const AGoodGameCharacter* Character, const FAbilityData& abilityData)
{
	if (abilityData.AllowedWeaponSlots == WeaponSlotsLengthBitMask) {
		// Any weapon is allowed but must have one.
		if (!Character->ActiveWeapon)
			return true;
	}
	else if (abilityData.AllowedWeaponSlots > 0) {
		// Check specific weapon type is allowed.
		if (!Character->ActiveWeapon || (abilityData.AllowedWeaponSlots & Character->ActiveWeapon->WeaponBitMask) == 0)
			return true;
	}

	return false;
}

bool AAbility::CanCast(const AGoodGameCharacter* Character, const EAbilityType AbilityTypeID)
{
	const UDataSingleton* gd = (UDataSingleton*)GEngine->GameSingleton;
	const FAbilityData& abilitydata = gd->AbilityTypes[(uint8)AbilityTypeID];

	/*return CanCastByData(Character, abilitydata);
}

bool AAbility::CanCastCached(const AGoodGameCharacter* Character, const TArray<FAbilityData>& AbilityTypes, const EAbilityType AbilityTypeID)
{
	return CanCastByData(Character, AbilityTypes[(uint8)AbilityTypeID]);
}

bool AAbility::CanCastByData(const AGoodGameCharacter* Character, const FAbilityData& abilitydata)
{*/
	if (abilitydata.bMustBeOnGround && !Character->GetCharacterMovement()->IsMovingOnGround())
		return false;

	if (abilitydata.bMustBeDry && Character->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Swimming)
		return false;

	if (abilitydata.bMustBeStationary && Character->GetVelocity().SizeSquared() >= 32.f)
		return false;

	if (AAbility::CantCastDueToWeaponSlot(Character, abilitydata))
		return false;

	// Maybe the weapon was forced to do something?
	if (Character->ActiveWeapon && Character->ActiveWeapon->IsBusy())
		return false;

	if (abilitydata.ManaCost > 0.f && Character->GetStaminaManaComponent()->GetMana() < Character->GetManaCost(abilitydata))
		return false;

	if (abilitydata.StaminaCost > 0.f && Character->GetStaminaManaComponent()->GetStamina() < Character->GetStaminaCost(abilitydata))
		return false;

	if (abilitydata.HealthCost > 0.f && Character->GetHealthComponent()->GetHealth() < abilitydata.HealthCost)
		return false;

	return true;
}

void AAbility::BeginPlay()
{
	Super::BeginPlay();

	GameSingleton = Cast<UDataSingleton>(GEngine->GameSingleton);

	if (GetNetMode() != NM_DedicatedServer) {
		const auto abilitydata = GameSingleton->AbilityTypes[(int)AbilityType];
		const auto schooltype = GameSingleton->SchoolTypes[(int)abilitydata.SchoolTypeID];

		LightComponent->SetLightColor(schooltype.Color);
	}

	if (!bInstantCast) {
		const FAbilityData& abilitydata = GameSingleton->AbilityTypes[(int)AbilityType];

		float casttime = abilitydata.CastingTime;

		if (abilitydata.bSpeedScaled) {
			const AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
			if (owner && owner->IsValidLowLevel()) {
				if (abilitydata.bSpeedScaledByCastingSpeed)
					casttime *= owner->CastingSpeedMultiplier;
				if (abilitydata.bSpeedScaledByAttackSpeed)
					casttime *= owner->AttackSpeedMultiplier;
				if (abilitydata.bSpeedScaledByBowSpeed)
					casttime *= owner->BowSpeedMultiplier;
				if (abilitydata.bSpeedScaledByWeapon && owner->ActiveWeapon)
					casttime *= owner->ActiveWeapon->WeaponSpeed;
			}
		}

		// Let client decide if there is no end time. Server will correct it.
		if (StartCastTime == 0.f)
			StartCastTime = WORLD_TIME;
		if (FinishCastTime == 0.f)
			FinishCastTime = StartCastTime + casttime;
	}
}

void AAbility::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	auto AudioComponent = FindComponentByClass<UAudioComponent>();
	if (AudioComponent)
		AudioComponent->Stop();

	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
	if (owner && owner->IsValidLowLevel() && owner->CastingAbility == this)
		owner->CastingAbility = nullptr;
}

void AAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsNetMode(NM_DedicatedServer)) {
		if (EndingReason > 0) {
			if (LightComponent->Intensity > 0.f)
				LightComponent->SetIntensity(FMath::Max(LightComponent->Intensity - DeltaTime * 16000.f, 0.f));
		}
		else if (LightComponent->Intensity < 6000.f)
			LightComponent->SetIntensity(FMath::Min(LightComponent->Intensity + DeltaTime * 24000.f, 6000.f));
	}

	if (HasAuthority() && !IsEnding()) {
		const AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());

		if (!owner)
			Destroy();
		else if (!CanCast(owner, AbilityType))
			EndAbility(EStopCastingReason::NotEnoughStats);
		else if (bInstantCast || !owner->bPressingUseAbility && WORLD_TIME >= FinishCastTime)
			EndAbility(EStopCastingReason::Success);
	}
}

void AAbility::SetOwner(AActor* NewOwner)
{
	// Can't set our owner to ANYTHING except a valid character.
	if (NewOwner && NewOwner->IsValidLowLevel() && NewOwner->IsA<AGoodGameCharacter>()) {
		Super::SetOwner(NewOwner);

		((AGoodGameCharacter*)NewOwner)->CastingAbility = this;
		AttachToActor(NewOwner, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	}
}

void AAbility::EndAbility(const EStopCastingReason Reason)
{
	EndingReason = (uint8)Reason;

	if (Reason == EStopCastingReason::Success) {
		AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
		if (owner) {
			const FAbilityData& abilitydata = GetAbilityData();

			owner->OnAbilityEnd(Reason);

			if (abilitydata.ManaCost > 0.f)
				owner->GetStaminaManaComponent()->TakeManaRaw(owner->GetManaCost(abilitydata));

			if (abilitydata.StaminaCost > 0.f)
				owner->GetStaminaManaComponent()->TakeStaminaRaw(owner->GetStaminaCost(abilitydata));

			if (abilitydata.HealthCost > 0.f)
				owner->GetHealthComponent()->TakeHealth(abilitydata.HealthCost);
		}
	}

	EndAbilityEffects();
	OnAbilityEnd(Reason);
}

void AAbility::EndAbilityEffects()
{
	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
	if (owner && owner->CastingAbility == this)
		owner->CastingAbility = nullptr;

	if (HasAuthority())
		SetLifeSpan(3.f);

	if (GetNetMode() != NM_DedicatedServer) {
		auto ParticleSystemComponent = FindComponentByClass<UParticleSystemComponent>();
		auto StaticMeshComponent = FindComponentByClass<UStaticMeshComponent>();
		auto AudioComponent = FindComponentByClass<UAudioComponent>();

		if (ParticleSystemComponent)
			ParticleSystemComponent->DeactivateSystem();

		if (StaticMeshComponent)
			StaticMeshComponent->SetHiddenInGame(true);

		if (AudioComponent)
			AudioComponent->FadeOut(0.25f, 0.f);

		/*LightComponent->SetAttenuationRadius(0.f);
		LightComponent->SetIntensity(0.f);*/

		if (EndingReason == (uint8)EStopCastingReason::Success && owner) {
			if (SuccessEffect) {
				FActorSpawnParameters spawnparams;
				spawnparams.Owner = owner;

				AActor* effect = GetWorld()->SpawnActor<AActor>(SuccessEffect, owner->GetActorLocation(), owner->GetActorRotation(), spawnparams);
				if (bSuccessEffectAttachToOwner && effect)
					effect->AttachToActor(owner, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
			}

			if (SuccessSound)
				UGGStatics::SpawnSoundOnActor(SuccessSound, owner);
			if (SuccessParticles)
				UGameplayStatics::SpawnEmitterAttached(SuccessParticles, owner->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
		}
	}
}

#if WITH_EDITOR
void AAbility::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	/*auto LightComponent = FindComponentByClass<UPointLightComponent>();
	if (!LightComponent)
	return;

	static ConstructorHelpers::FObjectFinder<UClass> C_DataSingleton(TEXT("/Game/Data/DataSingleton_BP.DataSingleton_BP_C"));
	const auto gd = C_DataSingleton.Object->GetDefaultObject<UDataSingleton>();

	if (gd && gd->AbilityTypes.Num() < (int)AbilityType) {
	const auto abilitydata = gd->AbilityTypes[(int)AbilityType];
	if (gd->SchoolTypes.Num() < (int)abilitydata.SchoolTypeID) {
	const auto schooltype = gd->SchoolTypes[(int)abilitydata.SchoolTypeID];

	LightComponent->LightColor = schooltype.Color;
	LightComponent->Intensity = 6000.f;
	LightComponent->AttenuationRadius = 300.f;
	LightComponent->bAffectTranslucentLighting = false;
	}
	}*/
}
#endif

float AAbility::GetHUDProgress() const
{
	if (bInstantCast || !bCanCancel)
		return 0.f;

	return FMath::Clamp((WORLD_TIME - StartCastTime) / (FinishCastTime - StartCastTime), 0.f, 1.f);
}

void AAbility::OnAbilityStart_Implementation() {}
void AAbility::OnAbilityEnd_Implementation(const EStopCastingReason Reason) {}

void AAbility::OnRep_Ending()
{
	if (EndingReason > 0)
		EndAbilityEffects();
}

void AAbility::OnRep_Owner()
{
	AActor* owner = GetOwner();
	if (owner && owner->IsA<AGoodGameCharacter>()) {
		AGoodGameCharacter* charowner = (AGoodGameCharacter*)owner;
		charowner->CastingAbility = this;
	}
}

void AAbility::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAbility, EndingReason);

	DOREPLIFETIME_CONDITION(AAbility, StartCastTime, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AAbility, FinishCastTime, COND_OwnerOnly);
}
