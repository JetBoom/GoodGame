#include "GoodGame.h"
#include "SECausticBolt.h"


ASECausticBolt::ASECausticBolt()
{
	if (TakeDamageElementModifiers.Num() == 0) {
		FDamageElementModifier modifier;
		modifier.ElementID = EDamageElement::Acid;
		modifier.Modifier = 0.f;
		modifier.bMultiply = false;

		TakeDamageElementModifiers.Push(modifier);
	}
}

void ASECausticBolt::SetNumberOfStacks(const uint8 NumStacks)
{
	Super::SetNumberOfStacks(NumStacks);

	TakeDamageElementModifiers[0].Modifier = NumStacks * 0.05f;

	auto character = Cast<AGoodGameCharacter>(GetOwner());
	if (character)
		character->InvalidateAttributeCalculations();
}
