#include "GoodGame.h"
#include "UnrealNetwork.h"
#include "GoodGameCharacter.h"
#include "Weapon.h"


AWeapon::AWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Movable);

	PrimaryActorTick.bCanEverTick = true;
	bCanBeDamaged = false;

	SetReplicates(true);
	SetReplicateMovement(false);

	SetActorHiddenInGame(false);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBitMask = FMath::Pow(2, (uint8)WeaponSlot);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AWeapon::IsBusy() const
{
	return StartAttackTime > KINDA_SMALL_NUMBER;
}

bool AWeapon::Holster(bool force)
{
	auto owner = (AGoodGameCharacter*)GetOwner();
	if (owner && owner->IsValidLowLevel() && (force || !IsBusy())) {
		owner->SetActiveWeapon(nullptr);

		return true;
	}

	return false;
}

void AWeapon::Deploy()
{
	auto owner = (AGoodGameCharacter*)GetOwner();
	if (owner && owner->IsValidLowLevel())
		owner->SetActiveWeapon(this);
}

void AWeapon::OnDeploy_Implementation()
{
	Show();

	if (GetNetMode() != NM_DedicatedServer) {
		if (DeployHolsterAudio) { DeployHolsterAudio->Stop(); DeployHolsterAudio->DestroyComponent(); DeployHolsterAudio = nullptr; }
		if (DeploySound) { DeployHolsterAudio = UGGStatics::SpawnSoundOnActor(DeploySound, GetOwner()); }
	}
}

void AWeapon::OnHolster_Implementation()
{
	Hide();

	ResetAttack();

	if (GetNetMode() != NM_DedicatedServer) {
		if (DeployHolsterAudio) { DeployHolsterAudio->Stop(); DeployHolsterAudio->DestroyComponent(); DeployHolsterAudio = nullptr; }
		if (HolsterSound) { DeployHolsterAudio = UGGStatics::SpawnSoundOnActor(HolsterSound, GetOwner()); }
	}
}

void AWeapon::OnAttackPressed_Implementation() {}
void AWeapon::OnAttackReleased_Implementation() {}
void AWeapon::OnCancelPressed_Implementation() {}

void AWeapon::Show()
{
	SetActorHiddenInGame(false);
}

void AWeapon::Hide()
{
	SetActorHiddenInGame(true);
}

float AWeapon::GetHUDProgress() const
{
	if (!bShouldEverShowHUDProgress || StartAttackTime < KINDA_SMALL_NUMBER)
		return 0.f;

	return FMath::Clamp((WORLD_TIME - StartAttackTime) / (FinishAttackTime - StartAttackTime), 0.f, 1.f);
}

void AWeapon::RefreshAttachment()
{
	// Ownership, attachments, and hiding are all already replicated on AActor by default.
	if (GetNetMode() >= NM_Client)
		return;

	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
	
	if (owner) {
		SetActorEnableCollision(false);

		static const auto attrules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		AttachToComponent(owner->GetMesh(), attrules, "EquipRightHand");
		
		if (owner->ActiveWeapon == this)
			Show();
		else
			Hide();
	}
	else {
		SetActorEnableCollision(true);

		static const auto detrules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		DetachFromActor(detrules);

		Show();
	}
}

void AWeapon::SetOwner(AActor* NewOwner)
{
	AActor* OldOwner = GetOwner();
	if (OldOwner == NewOwner || NewOwner && (!NewOwner->IsA<AGoodGameCharacter>() || !NewOwner->IsValidLowLevel()))
		return;

	Super::SetOwner(NewOwner);

	if (HasAuthority()) {
		if (OldOwner) {
			AGoodGameCharacter* oldcharowner = Cast<AGoodGameCharacter>(OldOwner);
			if (oldcharowner) {
				if (oldcharowner->ActiveWeapon == this)
					oldcharowner->SetActiveWeapon(nullptr);
				oldcharowner->Weapons.RemoveSingle(this);
			}
		}

		if (NewOwner) {
			AGoodGameCharacter* charowner = (AGoodGameCharacter*)NewOwner;
			if (!charowner->Weapons.Contains(this))
				charowner->Weapons.Push(this);
			if (!charowner->ActiveWeapon)
				charowner->SetActiveWeapon(this);
		}
	}

	RefreshAttachment();
}

void AWeapon::OnRep_StartAttackTime(float OldStartAttackTime)
{
	//if (FMath::Abs(OldStartAttackTime - StartAttackTime) > KINDA_SMALL_NUMBER) {
		if (StartAttackTime < KINDA_SMALL_NUMBER) {
			if (OldStartAttackTime > KINDA_SMALL_NUMBER)
				OnFinishAttack();
		}
		else if (StartAttackTime > KINDA_SMALL_NUMBER && OldStartAttackTime < KINDA_SMALL_NUMBER)
			OnStartAttack();
	//}
}

void AWeapon::OnStartAttack_Implementation() {}

void AWeapon::OnFinishAttack_Implementation() {}

void AWeapon::OnOwnerKilled_Implementation()
{
	ResetAttack();
}

void AWeapon::SetAttackTimes(const float AttackStart, const float AttackEnd)
{
	const float old = StartAttackTime;
	StartAttackTime = AttackStart;
	FinishAttackTime = AttackEnd;

	OnRep_StartAttackTime(old);
}

void AWeapon::ResetAttack()
{
	SetAttackTimes(0.f);
}

void AWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, StartAttackTime);
	DOREPLIFETIME(AWeapon, FinishAttackTime);
}
