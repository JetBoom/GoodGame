#include "GoodGame.h"
#include "UnrealNetwork.h"
#include "StatusEffect.h"


AStatusEffect::AStatusEffect()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SetActorEnableCollision(false);

	PrimaryActorTick.bCanEverTick = false;

	NetPriority = 1.f;
	NetUpdateFrequency = 20.f;
	SetReplicates(true);

	bCanBeDamaged = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

void AStatusEffect::OnOwnerTakeDamage_Implementation(float& DamageAmount, struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser, bool& bBlockDamage) {}
void AStatusEffect::PostOnOwnerTakeDamage_Implementation(float& DamageAmount, struct FDamageEvent const& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser) {}
void AStatusEffect::PostOnOwnerGiveDamage_Implementation(const float DamageAmount, struct FDamageEvent const& DamageEvent, const AGoodGameCharacter* Victim, const AActor* Inflictor) {}

void AStatusEffect::BeginPlay()
{
	Super::BeginPlay();

	SetTimeRemaining(InitialLifeSpan);

	RefreshAttachment();
}

void AStatusEffect::SetTimeRemaining(float time)
{
	if (!HasAuthority())
		return;

	if (time < 0.f)
		time = InitialLifeSpan;

	if (time <= 0.f) {
		SetLifeSpan(99999.f);
		EndTime = 0;
	}
	else {
		SetLifeSpan(time);
		EndTime = WORLD_TIME + GetLifeSpan();
	}
}

void AStatusEffect::SetNumberOfStacks(const uint8 NumStacks)
{
	NumberOfStacks = NumStacks;
}

// Owner is a replicated var of all actors, so just plop this status at the center of the owner when it changes.
void AStatusEffect::OnRep_Owner()
{
	Super::OnRep_Owner();

	RefreshAttachment();
}

void AStatusEffect::OnRep_NumberOfStacks()
{
	// TODO: update HUD status
}

void AStatusEffect::OnRep_EndTime()
{
	// TODO: update HUD status
}

void AStatusEffect::RefreshAttachment()
{
	AActor* owner = GetOwner();

	if (owner) {
		const static FAttachmentTransformRules attrules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		AttachToActor(owner, attrules);
	}
	else {
		const static FDetachmentTransformRules detrules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		DetachFromActor(detrules);
	}
}

void AStatusEffect::ApplyModifiers_Implementation(AGoodGameCharacter* Character)
{
	if (AttackSpeedModifier != 0.f && !bAttackSpeedModifierIsMultiplier)
		Character->AttackSpeedMultiplier += AttackSpeedModifier;

	if (BowSpeedModifier != 0.f && !bBowSpeedModifierIsMultiplier)
		Character->BowSpeedMultiplier += BowSpeedModifier;

	if (CastingSpeedModifier != 0.f && !bCastingSpeedModifierIsMultiplier)
		Character->CastingSpeedMultiplier += CastingSpeedModifier;

	if (MovementSpeedModifier != 0.f && !bMovementSpeedModifierIsMultiplier)
		Character->MovementSpeedMultiplier += MovementSpeedModifier;

	if (CoolDownModifier != 0.f && !bCoolDownModifierIsMultiplier)
		Character->CoolDownMultiplier += CoolDownModifier;

	if (TakeDamageModifier != 0.f && !bTakeDamageModifierIsMultiplier)
		Character->TakeDamageMultiplier += TakeDamageModifier;

	if (GiveDamageModifier != 0.f && !bGiveDamageModifierIsMultiplier)
		Character->GiveDamageMultiplier += GiveDamageModifier;

	if (StaminaUseModifier != 0.f && !bStaminaUseModifierIsMultiplier)
		Character->StaminaUseMultiplier += StaminaUseModifier;

	if (ManaUseModifier != 0.f && !bManaUseModifierIsMultiplier)
		Character->ManaUseMultiplier += ManaUseModifier;

	for (auto& modifier : GiveDamageElementModifiers) {
		if (!modifier.bMultiply)
			Character->GiveDamageMultiplierElement[(int)modifier.ElementID] += modifier.Modifier;
	}

	for (auto& modifier : TakeDamageElementModifiers) {
		if (!modifier.bMultiply)
			Character->TakeDamageMultiplierElement[(int)modifier.ElementID] += modifier.Modifier;
	}
}

void AStatusEffect::ApplyModifiersPassTwo_Implementation(AGoodGameCharacter* Character)
{
	if (AttackSpeedModifier != 0.f && bAttackSpeedModifierIsMultiplier)
		Character->AttackSpeedMultiplier *= AttackSpeedModifier;

	if (BowSpeedModifier != 0.f && bBowSpeedModifierIsMultiplier)
		Character->BowSpeedMultiplier *= BowSpeedModifier;

	if (CastingSpeedModifier != 0.f && bCastingSpeedModifierIsMultiplier)
		Character->CastingSpeedMultiplier *= CastingSpeedModifier;

	if (MovementSpeedModifier != 0.f && bMovementSpeedModifierIsMultiplier)
		Character->MovementSpeedMultiplier *= MovementSpeedModifier;

	if (CoolDownModifier != 0.f && bCoolDownModifierIsMultiplier)
		Character->CoolDownMultiplier *= CoolDownModifier;

	if (TakeDamageModifier != 0.f && bTakeDamageModifierIsMultiplier)
		Character->TakeDamageMultiplier *= TakeDamageModifier;

	if (GiveDamageModifier != 0.f && bGiveDamageModifierIsMultiplier)
		Character->GiveDamageMultiplier *= GiveDamageModifier;

	if (StaminaUseModifier != 0.f && bStaminaUseModifierIsMultiplier)
		Character->StaminaUseMultiplier *= StaminaUseModifier;

	if (ManaUseModifier != 0.f && bManaUseModifierIsMultiplier)
		Character->ManaUseMultiplier *= ManaUseModifier;

	for (const auto& modifier : GiveDamageElementModifiers) {
		if (modifier.bMultiply)
			Character->GiveDamageMultiplierElement[(int)modifier.ElementID] *= modifier.Modifier;
	}

	for (const auto& modifier : TakeDamageElementModifiers) {
		if (modifier.bMultiply)
			Character->TakeDamageMultiplierElement[(int)modifier.ElementID] *= modifier.Modifier;
	}
}

void AStatusEffect::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	bInvalidStatus = true;

	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
	if (owner)
		owner->RemoveStatus(this, true);
}

void AStatusEffect::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStatusEffect, EndTime);
	DOREPLIFETIME(AStatusEffect, NumberOfStacks);
}
