#pragma once

#include "Weapon.h"
#include "WeaponStaff.generated.h"


UCLASS()
class GOODGAME_API AWeaponStaff : public AWeapon
{
	GENERATED_BODY()
	
public:
	static const int BaseChargeTimeMS = 1250;
	static const int BaseManaCost = 5;

	AWeaponStaff();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StaffWeapon)
	USoundCue* ReleaseSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StaffWeapon)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_WasReleased, BlueprintReadOnly, Category = StaffWeapon)
	uint32 bWasReleased:1;

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnAttackPressed_Implementation() override;
	virtual void OnStartAttack_Implementation() override;
	virtual void OnFinishAttack_Implementation() override;
	virtual void OnCancelPressed_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StaffWeapon)
	UAudioComponent* ChargingAudio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = StaffWeapon)
	UParticleSystemComponent* ChargingParticles;

	UFUNCTION()
	virtual void OnRep_WasReleased();

};
