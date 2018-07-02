#include "GoodGame.h"
#include "HealthComponent.h"
#include "StaminaManaComponent.h"
#include "SEReviveSpell.h"


ASEReviveSpell::ASEReviveSpell() {}

void ASEReviveSpell::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!HasAuthority())
		return;

	AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
	if (owner && owner->IsDead()) {
		UHealthComponent* healthComponent = owner->GetHealthComponent();

		healthComponent->SetIsDead(false);
		healthComponent->SetHealth(FMath::Max(1.f, healthComponent->GetMaxHealth() * 0.2f));
		owner->GetStaminaManaComponent()->SetStamina(50.f);
		owner->GetStaminaManaComponent()->SetMana(50.f);
	}

	Super::EndPlay(EndPlayReason);
}
