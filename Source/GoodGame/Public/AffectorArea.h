#pragma once

#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "AffectorArea.generated.h"

UCLASS()
class GOODGAME_API AAffectorArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AAffectorArea();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AffectorArea)
	bool bOnlyOnBeginOverlap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AffectorArea)
	bool bAffectsOwner = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AffectorArea)
	float AffectorDelay = 0.75f;

	UFUNCTION(BlueprintCallable, Category = AffectorArea)
	void SetRadius(const float radius);

	UFUNCTION(BlueprintCallable, Category = AffectorArea)
	virtual void UpdateFromProjectile(const AProjectile* projectile);

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Repulsor, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collider;

	virtual void DoAffect(AActor* actor);
	virtual bool PassesFilter(const AActor* actor) const;

	TMap<AActor*, float> NextAffect;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

};
