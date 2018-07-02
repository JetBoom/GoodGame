#include "GoodGame.h"
#include "GoodGameCharacter.h"
#include "StaminaManaComponent.h"
#include "CharacterMovementComponentEx.h"

UCharacterMovementComponentEx::UCharacterMovementComponentEx(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UDamageType> C_FallDamageType(TEXT("/Game/DamageTypes/DT_Fall.DT_Fall_C"));
	FallDamageType = C_FallDamageType.Class;
}

void UCharacterMovementComponentEx::BeginPlay()
{
	Super::BeginPlay();

	DefaultMaxWalkSpeed = MaxWalkSpeed;
	MaxWalkSpeedCrouched = MaxWalkSpeed * 0.5f;
}

float UCharacterMovementComponentEx::GetMaxSpeed() const
{
	switch (MovementMode)
	{
		case MOVE_Walking: {
			const AGoodGameCharacter* owner = (AGoodGameCharacter*)GetCharacterOwner();
			if (!owner->GetStaminaManaComponent()->HasStaminaRaw(20.f))
				return MaxWalkSpeed * WalkExhaustedMultiplier;
			
			const float fdot = FVector::DotProduct(owner->GetActorForwardVector(), Velocity.GetSafeNormal2D());

			if (fdot < -0.5f)
				return MaxWalkSpeed * WalkBackwardsMultiplier;
			else if (fdot < 0.8f)
				return MaxWalkSpeed * WalkSideMultiplier;
		}
		case MOVE_NavWalking:
			return IsCrouching() ? MaxWalkSpeedCrouched : MaxWalkSpeed;
		case MOVE_Falling:
			return MaxWalkSpeed * MaxAirControlSpeedMultiplier;
		case MOVE_Swimming:
			return MaxSwimSpeed;
		case MOVE_Flying:
			return MaxFlySpeed;
		case MOVE_Custom:
			return MaxCustomMovementSpeed;
		case MOVE_None:
		default:
			return 0.f;
	}
}

bool UCharacterMovementComponentEx::DoJump(bool bReplayingMoves)
{
	if (Super::DoJump(bReplayingMoves)) {
		if (!bReplayingMoves) {
			const AGoodGameCharacter* ggc = (AGoodGameCharacter*)CharacterOwner;
			ggc->GetStaminaManaComponent()->TakeStamina(ggc->StaminaToJump);
		}

		if (GetWorld()->TimeSeconds <= BunnyHopAllowed) {
			BunnyHopAllowed = 0.f;
			Velocity += BunnyHopImpulse;
		}

		return true;
	}

	return false;
}

void UCharacterMovementComponentEx::SetMaxWalkSpeed(float speed)
{
	MaxWalkSpeed = speed;
	MaxWalkSpeedCrouched = speed * 0.5f;
}

void UCharacterMovementComponentEx::SetMaxWalkSpeedMultiplier(float multiplier)
{
	SetMaxWalkSpeed(DefaultMaxWalkSpeed * multiplier);
}

/*void UCharacterMovementComponentEx::SetDefaultMovementMode()
{
	// check for water volume
	if (CanEverSwim() && IsInWater())
	{
		SetMovementMode(DefaultWaterMovementMode);
	}
	else if (!CharacterOwner || MovementMode != DefaultLandMovementMode)
	{
		const float SavedVelocityZ = Velocity.Z;
		SetMovementMode(DefaultLandMovementMode);

		// Avoid 1-frame delay if trying to walk but walking fails at this location.
		if (MovementMode == MOVE_Walking && GetMovementBase() == NULL)
		{
			Velocity.Z = SavedVelocityZ; // Prevent temporary walking state from zeroing Z velocity.
			SetMovementMode(MOVE_Falling);
		}
	}
}*/

void UCharacterMovementComponentEx::SetPostLandedPhysics(const FHitResult& Hit)
{
	//const FVector PreImpactAccel = Acceleration + (IsFalling() ? FVector(0.f, 0.f, GetGravityZ()) : FVector::ZeroVector);
	const FVector PreImpactVelocity = Velocity;

	const auto oldMode = MovementMode;

	Super::SetPostLandedPhysics(Hit);

	if (MovementMode == MOVE_Walking) {
		// Pressed too soon, no hops for you.
		if (JumpInAirTime > KINDA_SMALL_NUMBER && GetWorld()->TimeSeconds < JumpInAirTime + 0.3f)
			return;

		FVector PreImpactNormal = PreImpactVelocity;
		PreImpactNormal.Normalize();
		FVector forward = PreImpactVelocity;
		forward.Z = 0.f;
		forward.Normalize();
		const FVector right = FVector::CrossProduct(-forward, FVector::UpVector);

		// Player will get this velocity added if they jump in the next few frames.
		FVector HopNormal = Hit.ImpactNormal;
		HopNormal.Z += 0.5f; // Skew towards less horizontal movement down hills and to nothing on slight slopes.
		HopNormal.Normalize();
		BunnyHopImpulse = PreImpactVelocity.MirrorByVector(HopNormal) - Velocity;
		BunnyHopImpulse.Z = 0.f;
		BunnyHopImpulse = BunnyHopImpulse.ProjectOnToNormal(forward) + BunnyHopImpulse.ProjectOnToNormal(right) * 0.2f;

		BunnyHopAllowed = GetWorld()->TimeSeconds + 0.15f;

		// Pressed right before landing, auto-hop.
		if (JumpInAirTime > KINDA_SMALL_NUMBER && BunnyHopAllowed - JumpInAirTime < 0.3f)
			CharacterOwner->Jump();

		if (CharacterOwner && CharacterOwner->HasAuthority()) {
			// The player receives more damage for head-on collisions but still receives a bit for lateral ones.
			const float impactDot = FMath::Clamp(FVector::DotProduct(PreImpactNormal, Hit.ImpactNormal) * -1.f, 0.f, 1.f);
			const float impactForce = PreImpactVelocity.Size() * (0.6f + impactDot * 0.4f);

			//UE_LOG(LogTemp, Log, TEXT("Impact force: %f"), impactForce);

			if (impactForce > 1000.f) {
				float fallDamage = FMath::Pow((impactForce - 1000.f) / 30.f, 1.5f);
				if (fallDamage > 5.f) {
					//UE_LOG(LogTemp, Log, TEXT("Uncapped fall damage: %f"), fallDamage);

					if (fallDamage > 100.f)
						fallDamage = 100.f;

					//UE_LOG(LogTemp, Log, TEXT("Capped fall damage: %f"), fallDamage);
					static const FDamageEvent damageEvent = FDamageEvent(FallDamageType);
					CharacterOwner->TakeDamage(fallDamage, damageEvent, nullptr, nullptr);
				}
			}
		}
	}

	JumpInAirTime = 0.f;
}
