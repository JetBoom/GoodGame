#pragma once

#include "GameFramework/Actor.h"
#include "GoodGameDamageType.h"
#include "EffectData.h"
#include "Projectile.generated.h"


UCLASS(BlueprintType)
class GOODGAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	TSubclassOf<AEffect> ProjectileExplosion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	float ProjectileDamage = 50.f;

	// Radius of the damage explosion. If 0 then only direct hits will damage.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile, meta = (ClampMin = 0.f))
	float ProjectileRadius = 500.f;

	// How aggressive the damage falls off. Higher values means less damage when at a distance. If 0 then falloff is disabled.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile, meta = (ClampMin = 0.f, ClampMax = 10.f))
	float ProjectileRadiusFalloff = 2.f;

	// Projectile will never cause less than this fraction of damage. Based on @ProjectileDamage.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile, meta = (ClampMin = 0.f, ClampMax = 1.f))
	float ProjectileMinimumDamage = 0.1f;

	// Immediately destroy this actor when hitting something, not giving a chance for effects like particle systems to smoothly deactivate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	bool ProjectileDestroyImmediately = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	TSubclassOf<UGoodGameDamageType> ProjectileDamageType = UGoodGameDamageType::StaticClass();

	UPROPERTY(Transient, BlueprintReadOnly, Category = Projectile, ReplicatedUsing = OnRep_Hit)
	FEffectData R_EffectData;

	// Should the instigator's attributes modify our damage?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool bAttributesModifiesDamage = true;

	// Should the instigator's weapon magnitude modify our damage?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool bWeaponsModifyDamage = true;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ULightComponent* LightComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	USoundCue* ProjectileHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ProjectileHitParticles;

	UPROPERTY(Transient, BlueprintReadOnly, Category = Projectile)
	bool bHit;

	UPROPERTY(Transient, BlueprintReadOnly, Category = Projectile)
	float DefaultLightIntensity;

	UFUNCTION()
	void OnRep_Hit();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//virtual void ProjectileHit(const FVector hitlocation, const FVector hitnormal, class AActor* OtherActor = nullptr);
	virtual void ProjectileHit(const FVector hitlocation, const FVector hitnormal, const FHitResult& SweepResult, AActor* OtherActor = nullptr);
	virtual void ProjectileHitEffects(const FVector hitlocation, const FVector hitnormal, class AActor* OtherActor = nullptr);
	virtual void ProjectileHitLogic(const FVector hitlocation, const FVector hitnormal, const FHitResult & SweepResult, class AActor* OtherActor = nullptr);

};