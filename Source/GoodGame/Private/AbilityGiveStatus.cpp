#include "GoodGame.h"
#include "GoodGameCharacter.h"
#include "CharacterAttributes.h"
#include "AbilityGiveStatus.h"

AAbilityGiveStatus::AAbilityGiveStatus() {}

void AAbilityGiveStatus::OnAbilityEnd_Implementation(const EStopCastingReason Reason)
{
	Super::OnAbilityEnd_Implementation(Reason);

	if (Reason == EStopCastingReason::Success && HasAuthority())
		((AGoodGameCharacter*)GetOwner())->AddStatusOfType(StatusEffectType);
}
