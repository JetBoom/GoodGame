#include "GoodGame.h"
#include "SEBarrage.h"


ASEBarrage::ASEBarrage()
{
}

void ASEBarrage::PostOnOwnerGiveDamage_Implementation(const float DamageAmount, struct FDamageEvent const& DamageEvent, const AGoodGameCharacter* Victim, const AActor* Inflictor)
{
	if (Victim != GetOwner() && DamageEvent.DamageTypeClass) {
		UGoodGameDamageType* DmgType = Cast<UGoodGameDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		if (DmgType && DmgType->bIsMelee)
			((AGoodGameCharacter*)GetOwner())->GetHealthComponent()->GiveHealth(DamageAmount * LeechRatio);
	}
}
