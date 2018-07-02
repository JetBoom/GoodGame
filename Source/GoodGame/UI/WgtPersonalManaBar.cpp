#include "GoodGame.h"
#include "StaminaManaComponent.h"
#include "PanelSlot.h"
#include "CanvasPanelSlot.h"
#include "WgtPersonalManaBar.h"


void UWgtPersonalManaBar::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	const auto character = Cast<AGoodGameCharacter>(GetOwningPlayerPawn());
	if (character) {
		const auto component = character->GetStaminaManaComponent();

		const float mana = component->GetMana();
		const int imana = FMath::RoundToInt(mana);

		if (imana != LastMana) {
			LastMana = imana;

			const float maxmana = component->GetMaxMana();
			const int imaxmana = FMath::RoundToInt(maxmana);

			static const auto fmt = FTextFormat::FromString("{0} / {1}");

			Text->SetText(FText::Format(fmt, FFormatArgumentValue(imana), FFormatArgumentValue(imaxmana)));

			((UCanvasPanelSlot*)Bar->Slot)->SetAnchors(FAnchors(0.f, 0.f, FMath::Clamp(mana / maxmana, 0.f, 1.f), 1.f));
		}
	}
}
