#include "GoodGame.h"
#include "AbilityFeatherFall.h"


void AAbilityFeatherFall::OnAbilityEnd_Implementation(const EStopCastingReason Reason)
{
	Super::OnAbilityEnd_Implementation(Reason);

	if (Reason == EStopCastingReason::Success) {
		AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
		if (owner)
			owner->GetCharacterMovement()->Velocity.Z = FMath::Max(owner->GetCharacterMovement()->Velocity.Z, 0.f);
	}
}
