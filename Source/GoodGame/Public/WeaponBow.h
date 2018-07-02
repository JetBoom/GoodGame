#pragma once

#include "Weapon.h"
#include "WeaponBow.generated.h"


UCLASS()
class GOODGAME_API AWeaponBow : public AWeapon
{
	GENERATED_BODY()

public:
	// How many milliseconds it takes to draw the bow.
	static const int BaseDrawTimeMS = 1250;
	static const int BaseStaminaCost = 5;
	
	AWeaponBow();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = BowWeapon)
	USoundCue* DrawSound;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = BowWeapon)
	USoundCue* DrawFinishSound;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = BowWeapon)
	USoundCue* ReleaseSound;

	// If set we'll use this damage type instead of the generic arrow damage type.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = BowWeapon)
	TSubclassOf<UGoodGameDamageType> OverrideDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = BowWeapon)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_WasReleased, BlueprintReadOnly, Category = BowWeapon)
	uint32 bWasReleased:1;

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnAttackPressed_Implementation() override;
	virtual void OnStartAttack_Implementation() override;
	virtual void OnFinishAttack_Implementation() override;
	virtual void OnCancelPressed_Implementation() override;

protected:
	// Set to the playing draw sound so we can cancel it if need be.
	UPROPERTY(BlueprintReadWrite, Category = BowWeapon)
	UAudioComponent* CurrentSound;

	UFUNCTION()
	virtual void OnRep_WasReleased();

};
