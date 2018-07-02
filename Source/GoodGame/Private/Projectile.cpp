#include "GoodGame.h"
#include "UnrealNetwork.h"
#include "GoodGameCharacter.h"
#include "GoodGameDamageType.h"
#include "EffectData.h"
#include "Projectile.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	InitialLifeSpan = 5.f;

	NetPriority = 2.f;
	NetUpdateFrequency = 1.f; // Increase this for projectiles that change trajectories.
	SetReplicates(true);
	SetReplicateMovement(true);

	bCanBeDamaged = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	Movement->bRotationFollowsVelocity = true;
	Movement->bInitialVelocityInLocalSpace = true;
	Movement->ProjectileGravityScale = 0.f;
	Movement->InitialSpeed = 2000.f;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Collider->SetEnableGravity(false);
	Collider->SetNotifyRigidBodyCollision(true);
	Collider->SetCanEverAffectNavigation(false);
	Collider->InitSphereRadius(8.f);
	Collider->SetCollisionProfileName("Projectile");
	Collider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Collider->bGenerateOverlapEvents = true;
	Collider->BodyInstance.bUseCCD = true;
	RootComponent = Collider;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// These are optional components so need to be fetched here.
	if (!IsNetMode(NM_DedicatedServer)) {
		ParticleSystemComponent = FindComponentByClass<UParticleSystemComponent>();
		StaticMeshComponent = FindComponentByClass<UStaticMeshComponent>();
		AudioComponent = FindComponentByClass<UAudioComponent>();
		LightComponent = FindComponentByClass<ULightComponent>();

		if (LightComponent) {
			DefaultLightIntensity = LightComponent->Intensity;
			if (LightComponent->MaxDrawDistance == 0.f) {
				LightComponent->MaxDrawDistance = 5000.f;
				LightComponent->MaxDistanceFadeRange = 4500.f;
			}
		}
	}
	
	//Collider->OnComponentHit.AddDynamic(this, &AProjectile::OnHit); // already called and create in-game delegate errors if uncommented???
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);

	// Projectiles modify their attributes when they're spawned if the instigator has character attributes that do so.
	if (bAttributesModifiesDamage && Instigator && HasAuthority()) {
		const AGoodGameCharacter* characterInstigator = Cast<AGoodGameCharacter>(Instigator);
		if (characterInstigator)
			characterInstigator->ModifyOutgoingDamage(ProjectileDamage, ProjectileDamageType, bWeaponsModifyDamage);
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bHit && LightComponent)
		LightComponent->SetIntensity(FMath::Max(LightComponent->Intensity - DeltaTime * DefaultLightIntensity * 6.f, 0.f));
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
		return;

	ProjectileHit(Hit.ImpactPoint + Hit.ImpactNormal, Hit.ImpactNormal, Hit, OtherActor);
}

void AProjectile::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!HasAuthority() || GetOwner() == OtherActor)
		return;

	if (bFromSweep)
		ProjectileHit(SweepResult.ImpactPoint + SweepResult.Normal, SweepResult.Normal, SweepResult, OtherActor);
	//else
	//	ProjectileHit(GetActorLocation(), GetActorRotation().Vector() * -1, OtherActor);
}

void AProjectile::ProjectileHit(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor)
{
	if (bHit)
		return;

	bHit = true;

	SetActorEnableCollision(false);

	if (HasAuthority()) {
		R_EffectData = FEffectData(hitlocation, hitnormal, OtherActor);

		if (ProjectileDestroyImmediately)
			Destroy();
		else
			OnRep_Hit();

		ForceNetUpdate();
	}

	Movement->InitialSpeed = 0.f;
	Movement->StopMovementImmediately();

	SetLifeSpan(3.f);

	if (GetNetMode() != NM_DedicatedServer) {
		if (ParticleSystemComponent)
			ParticleSystemComponent->DeactivateSystem();

		if (StaticMeshComponent)
			StaticMeshComponent->SetHiddenInGame(true);

		if (AudioComponent)
			AudioComponent->FadeOut(0.2f, 0.f);

		if (LightComponent) {
			LightComponent->SetIntensity(LightComponent->Intensity * 2.f);

			PrimaryActorTick.SetTickFunctionEnable(true);
		}

		ProjectileHitEffects(hitlocation, hitnormal);
	}

	ProjectileHitLogic(hitlocation, hitnormal, SweepResult, OtherActor);
}

void AProjectile::ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor)
{
	if (ProjectileRadius <= 0.f) {
		if (OtherActor)
			UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileDamage, SweepResult.Normal, SweepResult, GetInstigatorController(), this, ProjectileDamageType);
	}
	else {
		// Radius damage is applied as point damage to an actor we directly hit so that we can do critical back damage.
		TArray<AActor*> filter;
		if (OtherActor) {
			filter.Push(OtherActor);
			UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileDamage, SweepResult.Normal, SweepResult, GetInstigatorController(), this, ProjectileDamageType);
		}

		if (ProjectileRadiusFalloff <= 0.f)
			UGameplayStatics::ApplyRadialDamage(GetWorld(), ProjectileDamage, hitlocation, ProjectileRadius, ProjectileDamageType, filter, this, GetInstigatorController(), true, ECC_AOE);
		else
			UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), ProjectileDamage, ProjectileMinimumDamage * ProjectileDamage, hitlocation, FMath::Min(ProjectileRadius * 0.1f, 64.f), ProjectileRadius, ProjectileRadiusFalloff, ProjectileDamageType, filter, this, GetInstigatorController(), ECC_AOE);
	}
}

void AProjectile::ProjectileHitEffects(const FVector hitlocation, const FVector hitnormal, class AActor* OtherActor)
{
	if (ProjectileExplosion) {
		FActorSpawnParameters spawnparams;
		spawnparams.Instigator = Instigator;

		GetWorld()->SpawnActor<AActor>(ProjectileExplosion, hitlocation, hitnormal.Rotation(), spawnparams);
	}

	if (ProjectileHitSound)
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ProjectileHitSound, hitlocation, hitnormal.Rotation());
	if (ProjectileHitParticles)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileHitParticles, hitlocation, hitnormal.Rotation(), true);
}

void AProjectile::OnRep_Hit()
{
	ProjectileHit(R_EffectData.Location, R_EffectData.Normal, FHitResult(R_EffectData.Actor, nullptr, R_EffectData.Location, R_EffectData.Normal), R_EffectData.Actor);
}

void AProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile, R_EffectData);
}
