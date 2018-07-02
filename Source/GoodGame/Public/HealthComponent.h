// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOODGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(replicated, EditAnywhere, BlueprintReadWrite, Category = Health)
	float HealthBase = 1.f;

	UPROPERTY(replicated, EditAnywhere, BlueprintReadWrite, Category = Health)
	float HealthMax = 1.f;

	UPROPERTY(replicated, EditAnywhere, BlueprintReadWrite, Category = Health)
	float HealthRate = 0.f;

	UPROPERTY(replicated, BlueprintReadWrite, Category = Health)
	float HealthUpdateTime = 0.f;
	
	UFUNCTION(BlueprintCallable, Category = Health)
	void SetHealth(float health);

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = Health)
	void SetMaxHealth(const float& maxhealth);

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = Health)
	void TakeHealth(const float& health, AActor* attacker = nullptr, AActor* inflictor = nullptr);

	UFUNCTION(BlueprintCallable, Category = Health)
	void GiveHealth(const float& health, AController* healer = nullptr);

	UFUNCTION(BlueprintCallable, Category = Health)
	void SetIsDead(const bool& dead);

	UFUNCTION(BlueprintCallable, Category = Health)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, Category = Health)
	void SetLastAttacker(AActor* attacker, AActor* inflictor = nullptr);

	virtual void BeginPlay() override;
	
	virtual bool IsSupportedForNetworking() const override { return true; }

	// Calls OnKilled and OnRevived for clients.
	UFUNCTION()
	void OnRep_IsDead();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadWrite, Category = Health)
	uint32 bIsDead:1;

	UPROPERTY(BlueprintReadWrite, Category = Health)
	TWeakObjectPtr<AActor> LastAttacker;

	UPROPERTY(BlueprintReadWrite, Category = Health)
	float LastAttackerSetTime;

	UPROPERTY(BlueprintReadWrite, Category = Health)
	TWeakObjectPtr<AActor> LastInflictor;

};
