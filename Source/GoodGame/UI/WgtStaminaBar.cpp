#include "GoodGame.h"
#include "StaminaManaComponent.h"
#include "PanelSlot.h"
#include "CanvasPanelSlot.h"
#include "WgtStaminaBar.h"


void UWgtStaminaBar::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	const auto character = Cast<AGoodGameCharacter>(GetOwningPlayerPawn());
	if (character) {
		const auto component = character->GetStaminaManaComponent();

		const float stamina = component->GetStamina();
		const int istamina = FMath::RoundToInt(stamina);

		if (istamina != LastStamina) {
			LastStamina = istamina;

			const float maxstamina = component->GetMaxStamina();
			const int imaxstamina = FMath::RoundToInt(maxstamina);

			static const auto fmt = FTextFormat::FromString("{0} / {1}");

			Text->SetText(FText::Format(fmt, FFormatArgumentValue(istamina), FFormatArgumentValue(imaxstamina)));

			((UCanvasPanelSlot*)Bar->Slot)->SetAnchors(FAnchors(0.f, 0.f, FMath::Clamp(stamina / maxstamina, 0.f, 1.f), 1.f));
		}
	}
}
