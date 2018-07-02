#include "GoodGame.h"
#include "WgtTargetHealthBar.h"


void UWgtTargetHealthBar::SetHealthPercentage(uint8 percentage)
{
	if (percentage != LastPercentage) {
		if (percentage > 100)
			percentage = 100;

		static const auto fmt = TEXT("%d%%");
		Text->SetText(FText::FromString(FString::Printf(fmt, percentage)));
		Bar->Brush.ImageSize.X = (float)percentage * 1.81f;
	}
}
