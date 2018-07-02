#include "GoodGame.h"
#include "WgtFPS.h"


void UWgtFPS::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	FrameTime += DeltaTime;
	++Frames;
	if (FrameTime < 1.f)
		return;

	const auto FPS = FMath::CeilToInt(Frames / FrameTime);

	FrameTime = 0.f;
	Frames = 0;

	if (FPS != LastFPS) {
		LastFPS = FPS;

		static const auto fmt = FTextFormat::FromString("{0} FPS");
		Text->SetText(FText::Format(fmt, FFormatArgumentValue(FPS)));
	}
}
