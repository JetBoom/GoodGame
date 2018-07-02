
#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterMovementComponentEx.generated.h"

UCLASS()
class GOODGAME_API UCharacterMovementComponentEx : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UCharacterMovementComponentEx(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category = "Character Movement: Jumping / Falling")
	float MaxAirControlSpeedMultiplier = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Jumping / Falling")
	TSubclassOf<UDamageType> FallDamageType;

	UPROPERTY(EditAnywhere, Category = "Walking")
	float WalkBackwardsMultiplier = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Walking")
	float WalkSideMultiplier = 0.75f;

	UPROPERTY(EditAnywhere, Category = "Walking")
	float WalkExhaustedMultiplier = 0.33f;
	
	UPROPERTY(Transient)
	float DefaultMaxWalkSpeed;

	/*UPROPERTY(Transient)
	uint16 RepulsorCount;*/

	UFUNCTION()
	void SetMaxWalkSpeed(float speed);

	UFUNCTION()
	void SetMaxWalkSpeedMultiplier(float multiplier);

	virtual float GetMaxSpeed() const override;
	virtual void BeginPlay() override;
	virtual bool DoJump(bool bReplayingMoves) override;

	float JumpInAirTime;

protected:
	virtual void SetPostLandedPhysics(const FHitResult& Hit) override;
	
	FVector BunnyHopImpulse;
	float BunnyHopAllowed;

};
