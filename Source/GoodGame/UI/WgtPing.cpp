#include "GoodGame.h"
#include "WgtPing.h"


void UWgtPing::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	UpdateTimeLeft -= DeltaTime;
	if (UpdateTimeLeft > 0.f)
		return;
	UpdateTimeLeft = 1.f;

	const auto player = GetOwningPlayer();
	if (!player)
		return;

	const auto state = player->PlayerState;
	if (!state)
		return;

	const auto ping = FMath::CeilToInt(state->ExactPing);
	if (ping != LastPing) {
		LastPing = ping;

		static const auto fmt = FTextFormat::FromString("{0} ms");
		Text->SetText(FText::Format(fmt, FFormatArgumentValue(ping)));
	}
}
