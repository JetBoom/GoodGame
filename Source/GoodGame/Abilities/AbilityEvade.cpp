#include "GoodGame.h"
#include "GoodGamePlayerController.h"
#include "AbilityEvade.h"

AAbilityEvade::AAbilityEvade()
{
	AbilityType = EAbilityType::Evade;
	bInstantCast = true;
}

void AAbilityEvade::OnAbilityEnd_Implementation(const EStopCastingReason Reason)
{
	Super::OnAbilityEnd_Implementation(Reason);

	if (Reason == EStopCastingReason::Success) {
		APawn* owner = (APawn*)GetOwner();
		if (owner) {
			const AGoodGameCharacter* charowner = (AGoodGameCharacter*)owner;
			const AGoodGamePlayerController* controller = (AGoodGamePlayerController*)charowner->Controller;
			if (controller) {
				UCharacterMovementComponent* movement = charowner->GetCharacterMovement();

				// Use input direction, defaulting to backwards if using nothing.
				FVector dir = movement->Velocity;
				dir.Z = 0.f;
				if (dir.SizeSquared2D() <= 16.f)
					dir = charowner->GetActorForwardVector() * -1;
				else
					dir.Normalize();

				// Project the direction on the current ground normal.
				dir = /*(*/FVector::VectorPlaneProject(dir, movement->CurrentFloor.HitResult.ImpactNormal)/* + dir) / 2.f*/;

				// Add a bit of upwards angle.
				FRotator rot = dir.Rotation();
				rot.Add(ImpulseAngle, 0, 0);
				dir = rot.Vector();

				// Override velocity.
				movement->Velocity = FVector::ZeroVector;
				movement->AddImpulse(dir * ImpulseMagnitude, true);
			}
		}
	}
}
