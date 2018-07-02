

#pragma once

#include "GameFramework/Pawn.h"
#include "HasHealth.h"
#include "GoodGamePawn.generated.h"

UCLASS()
class GOODGAME_API AGoodGamePawn : public APawn, public IHasHealth
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* HealthComponent;

public:
	// Sets default values for this pawn's properties
	AGoodGamePawn(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Health)
	class UHealthComponent* GetHealthComponent() const;
	virtual class UHealthComponent* GetHealthComponent_Implementation() const override { return HealthComponent; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Health)
	bool OnKilled(AActor* attacker = nullptr, AActor* inflictor = nullptr);
	virtual bool OnKilled_Implementation(AActor* attacker = nullptr, AActor* inflictor = nullptr) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Health)
	void OnRevived();
	virtual void OnRevived_Implementation() override;*/

	UFUNCTION(BlueprintCallable, Category = Health)
	UHealthComponent* GetHealthComponent() const { return HealthComponent; };

	// IHasHealth
	virtual float GetHealth() const override { return HealthComponent->GetHealth(); };
	virtual float GetMaxHealth() const override { return HealthComponent->HealthMax; };
	virtual bool OnKilled(AActor* attacker = nullptr, AActor* inflictor = nullptr) override;
	virtual void OnRevived() override;
	// End IHasHealth

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
};
