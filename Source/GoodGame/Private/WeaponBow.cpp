#include "GoodGame.h"
#include "GoodGameCharacter.h"
#include "StaminaManaComponent.h"
#include "WeaponBow.h"


AWeaponBow::AWeaponBow()
{
	bShouldEverShowHUDProgress = true;

	WeaponType = EWeaponTypes::Bow;
	WeaponSlot = EWeaponSlots::Bow;

	static ConstructorHelpers::FObjectFinder<USoundCue> drawcue(TEXT("/Game/Audio/Bow/SC_Bow_Draw"));
	static ConstructorHelpers::FObjectFinder<USoundCue> releasecue(TEXT("/Game/Audio/Bow/SC_Bow_Fire"));
	static ConstructorHelpers::FObjectFinder<UClass> projectileclass(TEXT("/Game/Blueprints/Projectiles/PRJ_Arrow_BP.PRJ_Arrow_BP_C"));

	DrawSound = drawcue.Object;
	ReleaseSound = releasecue.Object;
	ProjectileClass = projectileclass.Object;
}

void AWeaponBow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (StartAttackTime < KINDA_SMALL_NUMBER)
		return;

	if (!GetOwner()) {
		bWasReleased = false;
		ResetAttack();
		return;
	}
	
	if (!IsOwnerOrAuthority())
		return;

	AGoodGameCharacter* owner = (AGoodGameCharacter*)GetOwner();
	if (!owner)
		return;

	if (!owner->GetStaminaManaComponent()->HasStamina(BaseStaminaCost)) {
		ResetAttack();
		return;
	}

	if (owner->bPressingUseWeapon || WORLD_TIME < FinishAttackTime)
		return;

	owner->GetStaminaManaComponent()->TakeStamina(BaseStaminaCost);

	ResetAttack();
	
	if (HasAuthority()) {
		bWasReleased = true;
		OnRep_WasReleased();

		static FActorSpawnParameters spawnparams;
		spawnparams.Instigator = owner;
		spawnparams.Owner = owner;

		FVector pos;
		FRotator ang;
		owner->GetShootTransform(pos, ang);

		GetWorld()->SpawnActor<AProjectile>(ProjectileClass, pos, ang, spawnparams);
	}
}

void AWeaponBow::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponBow::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetNetMode() != NM_DedicatedServer && CurrentSound) {
		CurrentSound->Stop();
		CurrentSound->DestroyComponent();
	}
}

void AWeaponBow::OnCancelPressed_Implementation()
{
	if (StartAttackTime > KINDA_SMALL_NUMBER && WORLD_TIME >= StartAttackTime + 0.25f)
		ResetAttack();
}

void AWeaponBow::OnAttackPressed_Implementation()
{
	if (IsBusy())
		return;

	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
	if (owner && !owner->IsDead() && !owner->CastingAbility && owner->GetStaminaManaComponent()->HasStamina(BaseStaminaCost)) {
		const float curtime = WORLD_TIME;
		if (curtime >= owner->GlobalCooldown) {
			bWasReleased = false;

			const static float BaseDrawTime = (float)BaseDrawTimeMS / 1000.f;

			SetAttackTimes(curtime, curtime + BaseDrawTime / owner->BowSpeedMultiplier / WeaponSpeed);
		}
	}
}

void AWeaponBow::OnStartAttack_Implementation()
{
	Super::OnStartAttack_Implementation();

	if (!IsNetMode(NM_DedicatedServer) && DrawSound) {
		AActor* owner = GetOwner();
		if (owner)
			CurrentSound = UGGStatics::SpawnSoundOnActor(DrawSound, owner);
	}
}

void AWeaponBow::OnFinishAttack_Implementation()
{
	Super::OnFinishAttack_Implementation();

	if (!IsNetMode(NM_DedicatedServer) && CurrentSound) {
		CurrentSound->Stop();
		CurrentSound->DestroyComponent();
		CurrentSound = nullptr;
	}
}

void AWeaponBow::OnRep_WasReleased()
{
	if (bWasReleased && ReleaseSound && !IsNetMode(NM_DedicatedServer)) {
		const auto owner = GetOwner();
		if (owner)
			UGGStatics::SpawnSoundOnActor(ReleaseSound, owner);
	}
}

void AWeaponBow::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBow, bWasReleased);
}
