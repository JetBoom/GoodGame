#include "GoodGame.h"
#include "UnrealNetwork.h"
#include "StaminaManaComponent.h"


UStaminaManaComponent::UStaminaManaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bHighPriority = true;
	PrimaryComponentTick.TickInterval = 1.0f; //0.f;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;

	bReplicates = true;
}

void UStaminaManaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Mana += ManaRate * DeltaTime;
	Stamina += StaminaRate * DeltaTime;
	if (Mana > ManaMax)
		Mana = ManaMax;
	if (Stamina > StaminaMax)
		Stamina = StaminaMax;

	/*if (GetWorld()->TimeSeconds >= NextRep) {
		ManaRep = Mana;
		StaminaRep = Stamina;

		NextRep = GetWorld()->TimeSeconds + 1.f;
	}*/
}

void UStaminaManaComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() < NM_Client) {
		if (Stamina == 0.f)
			Stamina = StaminaMax;

		if (Mana == 0.f)
			Mana = ManaMax;
	}
	else
		SetComponentTickEnabled(false);
}

/*void UStaminaManaComponent::OnRep_ManaRep()
{
	Mana = ManaRep;
}

void UStaminaManaComponent::OnRep_ManaRep()
{
	Stamina = StaminaRep;
}*/

void UStaminaManaComponent::SetStamina(float stam)
{
	Stamina = FMath::Clamp(stam, 0.f, StaminaMax);
}

void UStaminaManaComponent::TakeStaminaRaw(float stam)
{
	Stamina = FMath::Max(Stamina - stam, 0.f);
}

void UStaminaManaComponent::TakeStamina(float stam)
{
	Stamina = FMath::Max(Stamina - stam * GetStaminaUseMultiplier(), 0.f);
}

bool UStaminaManaComponent::TakeStaminaIfHas(float stam)
{
	if (HasStamina(stam)) {
		TakeStamina(stam);
		return true;
	}

	return false;
}

void UStaminaManaComponent::GiveStamina(float stam)
{
	Stamina = FMath::Min(Stamina + stam, StaminaMax);
}

bool UStaminaManaComponent::HasStaminaRaw(float stam) const
{
	return Stamina >= stam;
}

bool UStaminaManaComponent::HasStamina(float stam) const
{
	return Stamina >= stam * GetStaminaUseMultiplier();
}

float UStaminaManaComponent::GetStamina() const
{
	return Stamina;
}

void UStaminaManaComponent::SetMaxStamina(float stam)
{
	StaminaMax = FMath::Max(stam, 1.f);
}

float UStaminaManaComponent::GetMaxStamina() const
{
	return StaminaMax;
}

float UStaminaManaComponent::GetStaminaUseMultiplier() const
{
	const AGoodGameCharacter* owner = (AGoodGameCharacter*)GetOwner();
	if (owner)
		return owner->StaminaUseMultiplier;

	return 1.f;
}

void UStaminaManaComponent::SetMana(float man)
{
	Mana = FMath::Clamp(man, 0.f, ManaMax);
}

void UStaminaManaComponent::TakeManaRaw(float man)
{
	Mana = FMath::Max(Mana - man, 0.f);
}

void UStaminaManaComponent::TakeMana(float man)
{
	Mana = FMath::Max(Mana - man * GetManaUseMultiplier(), 0.f);
}

bool UStaminaManaComponent::TakeManaIfHas(float man)
{
	if (HasMana(man)) {
		TakeMana(man);
		return true;
	}

	return false;
}

void UStaminaManaComponent::GiveMana(float man)
{
	Mana = FMath::Min(Mana + man, ManaMax);
}

bool UStaminaManaComponent::HasManaRaw(float man) const
{
	return Mana >= man;
}

bool UStaminaManaComponent::HasMana(float man) const
{
	return Mana >= man * GetManaUseMultiplier();
}

float UStaminaManaComponent::GetMana() const
{
	return Mana;
}

void UStaminaManaComponent::SetMaxMana(float man)
{
	ManaMax = FMath::Max(man, 1.f);
}

float UStaminaManaComponent::GetMaxMana() const
{
	return ManaMax;
}

float UStaminaManaComponent::GetManaUseMultiplier() const
{
	const AGoodGameCharacter* owner = (AGoodGameCharacter*)GetOwner();
	if (owner)
		return owner->ManaUseMultiplier;

	return 1.f;
}

void UStaminaManaComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*DOREPLIFETIME_CONDITION(UStaminaManaComponent, StaminaRep, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UStaminaManaComponent, ManaRep, COND_OwnerOnly);*/
	DOREPLIFETIME_CONDITION(UStaminaManaComponent, Stamina, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UStaminaManaComponent, Mana, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UStaminaManaComponent, ManaMax, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UStaminaManaComponent, StaminaMax, COND_OwnerOnly);
}
