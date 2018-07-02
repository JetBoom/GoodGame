#pragma once

#include "Components/ActorComponent.h"
#include "StaminaManaComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOODGAME_API UStaminaManaComponent : public UActorComponent
{
	GENERATED_BODY()

	float NextRep;

public:
	// Sets default values for this component's properties
	UStaminaManaComponent();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Stamina)
	float Stamina;

	/*UPROPERTY(ReplicatedUsing = OnRep_StaminaRep, VisibleAnywhere, BlueprintReadOnly, Category = Stamina)
	float StaminaRep;*/

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Stamina)
	float StaminaMax = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stamina)
	float StaminaRate = 1.f;

	UFUNCTION(BlueprintCallable, Category = Stamina)
	void SetStamina(float stam);

	UFUNCTION(BlueprintCallable, Category = Stamina)
	bool HasStamina(float stam) const;

	UFUNCTION(BlueprintCallable, Category = Stamina)
	bool HasStaminaRaw(float stam) const;

	UFUNCTION(BlueprintCallable, Category = Stamina)
	void TakeStamina(float stam);

	UFUNCTION(BlueprintCallable, Category = Stamina)
	bool TakeStaminaIfHas(float stam);

	UFUNCTION(BlueprintCallable, Category = Stamina)
	void TakeStaminaRaw(float stam);

	UFUNCTION(BlueprintCallable, Category = Stamina)
	void GiveStamina(float stam);

	UFUNCTION(BlueprintCallable, Category = Stamina)
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = Stamina)
	void SetMaxStamina(float maxstam);

	UFUNCTION(BlueprintCallable, Category = Stamina)
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = Stamina)
	float GetStaminaUseMultiplier() const;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Mana)
	float Mana;

	/*UPROPERTY(ReplicatedUsing = OnRep_ManaRep, VisibleAnywhere, BlueprintReadOnly, Category = Stamina)
	float ManaRep;*/

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Mana)
	float ManaMax = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mana)
	float ManaRate = 1.f;

	UFUNCTION(BlueprintCallable, Category = Mana)
	void SetMana(float man);

	UFUNCTION(BlueprintCallable, Category = Mana)
	bool HasMana(float man) const;

	UFUNCTION(BlueprintCallable, Category = Mana)
	bool HasManaRaw(float man) const;

	UFUNCTION(BlueprintCallable, Category = Mana)
	void TakeMana(float man);

	UFUNCTION(BlueprintCallable, Category = Mana)
	bool TakeManaIfHas(float man);

	UFUNCTION(BlueprintCallable, Category = Mana)
	void TakeManaRaw(float man);

	UFUNCTION(BlueprintCallable, Category = Mana)
	void GiveMana(float man);

	UFUNCTION(BlueprintCallable, Category = Mana)
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = Mana)
	void SetMaxMana(float maxman);

	UFUNCTION(BlueprintCallable, Category = Mana)
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = Mana)
	float GetManaUseMultiplier() const;

	/*UFUNCTION()
	void OnRep_ManaRep();

	UFUNCTION()
	void OnRep_StaminaRep();*/

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool IsSupportedForNetworking() const override { return true; }
};
