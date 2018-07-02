#pragma once

#include "GameFramework/Actor.h"
#include "Repulsor.generated.h"

UENUM(BlueprintType)
enum class ERepulsorType : uint8
{
	Radial,
	FlowThrough
};

UCLASS()
class GOODGAME_API ARepulsor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARepulsor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Repulsor)
	ERepulsorType RepulsorType = ERepulsorType::Radial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Repulsor)
	float Force = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Repulsor)
	float DistanceScaleExp = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Repulsor)
	float MinFlowZ = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Repulsor)
	uint32 bOnlyOnBeginOverlap:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Repulsor)
	uint32 bAffectsOwner:1;

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Repulsor, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collider;

};
