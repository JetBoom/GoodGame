#include "GoodGame.h"
#include "HealthComponent.h"
#include "PanelSlot.h"
#include "CanvasPanelSlot.h"
#include "WgtHealthBar.h"


void UWgtHealthBar::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	const auto character = Cast<AGoodGameCharacter>(GetOwningPlayerPawn());
	if (character) {
		const auto component = character->GetHealthComponent();

		const float health = component->GetHealth();
		const int ihealth = FMath::RoundToInt(health);

		if (ihealth != LastHealth) {
			LastHealth = ihealth;

			const float maxhealth = component->GetMaxHealth();
			const int imaxhealth = FMath::RoundToInt(maxhealth);

			static const auto fmt = FTextFormat::FromString("{0} / {1}");

			Text->SetText(FText::Format(fmt, FFormatArgumentValue(ihealth), FFormatArgumentValue(imaxhealth)));

			((UCanvasPanelSlot*)Bar->Slot)->SetAnchors(FAnchors(0.f, 0.f, FMath::Clamp(health / maxhealth, 0.f, 1.f), 1.f));
		}
	}
}
