#include "GoodGame.h"
#include "GoodGameCharacter.h"
#include "StaminaManaComponent.h"
#include "WeaponStaff.h"


AWeaponStaff::AWeaponStaff()
{
	bShouldEverShowHUDProgress = true;

	WeaponType = EWeaponTypes::Staff;
	WeaponSlot = EWeaponSlots::Staff;

	static ConstructorHelpers::FObjectFinder<USoundCue> aura(TEXT("/Game/Audio/StaffBolt/SC_StaffBolt_Charging"));
	static ConstructorHelpers::FObjectFinder<USoundCue> releasecue(TEXT("/Game/Audio/StaffBolt/SC_StaffBolt_Fire"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> particles(TEXT("/Game/Particles/P_StaffBoltCharge"));
	static ConstructorHelpers::FObjectFinder<UClass> projectileclass(TEXT("/Game/Blueprints/Projectiles/PRJ_StaffBolt_BP.PRJ_StaffBolt_BP_C"));

	ChargingAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ChargingSound"));
	ChargingAudio->SetSound(aura.Object);
	ChargingAudio->bAutoActivate = false;
	ChargingAudio->SetupAttachment(RootComponent);

	ChargingParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ChargingParticles"));
	ChargingParticles->SetTemplate(particles.Object);
	ChargingParticles->bAutoActivate = false;
	ChargingParticles->SetupAttachment(RootComponent);

	ProjectileClass = projectileclass.Object;
	ReleaseSound = releasecue.Object;
}

void AWeaponStaff::OnCancelPressed_Implementation()
{
	if (StartAttackTime > KINDA_SMALL_NUMBER && WORLD_TIME >= StartAttackTime + 0.25f)
		ResetAttack();
}

void AWeaponStaff::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (StartAttackTime < KINDA_SMALL_NUMBER)
		return;

	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());

	if (!owner) {
		bWasReleased = false;
		ResetAttack();
		return;
	}

	if (!IsOwnerOrAuthority())
		return;

	if (!owner->GetStaminaManaComponent()->HasMana(BaseManaCost)) {
		ResetAttack();
		return;
	}

	if (owner->bPressingUseWeapon || WORLD_TIME < FinishAttackTime)
		return;

	owner->GetStaminaManaComponent()->TakeMana(BaseManaCost);

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

void AWeaponStaff::OnAttackPressed_Implementation()
{
	if (IsBusy())
		return;

	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
	if (owner && !owner->IsDead() && !owner->CastingAbility && owner->GetStaminaManaComponent()->HasMana(BaseManaCost)) {
		const float curtime = WORLD_TIME;
		if (curtime >= owner->GlobalCooldown) {
			bWasReleased = false;

			const static float BaseChargeTime = (float)BaseChargeTimeMS / 1000.f;

			SetAttackTimes(curtime, curtime + BaseChargeTime / owner->CastingSpeedMultiplier / WeaponSpeed);
		}
	}
}

void AWeaponStaff::OnStartAttack_Implementation()
{
	Super::OnStartAttack_Implementation();

	if (GetNetMode() == NM_DedicatedServer)
		return;

	AActor* owner = GetOwner();
	if (owner) {
		static const FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		if (ChargingAudio)
			ChargingAudio->AttachToComponent(owner->GetRootComponent(), rules, NAME_None);
		/*if (ChargingParticles)
			ChargingParticles->AttachToComponent(owner->GetRootComponent(), rules, NAME_None);*/
	}

	if (ChargingAudio)
		ChargingAudio->Play();
	if (ChargingParticles)
		ChargingParticles->SetActive(true, true);
}

void AWeaponStaff::OnFinishAttack_Implementation()
{
	Super::OnFinishAttack_Implementation();

	if (GetNetMode() == NM_DedicatedServer)
		return;

	if (ChargingAudio)
		ChargingAudio->FadeOut(0.2f, 0.f);
	if (ChargingParticles)
		ChargingParticles->DeactivateSystem();
}

void AWeaponStaff::OnRep_WasReleased()
{
	if (GetNetMode() == NM_DedicatedServer)
		return;

	if (bWasReleased && ReleaseSound) {
		const auto owner = GetOwner();
		if (owner)
			UGGStatics::SpawnSoundOnActor(ReleaseSound, owner);
	}
}

void AWeaponStaff::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponStaff::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetNetMode() != NM_DedicatedServer) {
		if (ChargingAudio)
			ChargingAudio->Stop();
		if (ChargingParticles)
			ChargingParticles->DeactivateSystem();
	}
}

void AWeaponStaff::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponStaff, bWasReleased);
}
