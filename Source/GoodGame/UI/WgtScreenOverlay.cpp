#include "GoodGame.h"
#include "WgtScreenOverlay.h"


void UWgtScreenOverlay::BeginOverlay(const float& time)
{
	if (TimeRemaining < time)
		TimeRemaining = time;

	SetVisibility(ESlateVisibility::HitTestInvisible);

	FLinearColor col = ScreenImage->ColorAndOpacity;
	col.A = BaseOpacity;
	ScreenImage->SetColorAndOpacity(col);
}

void UWgtScreenOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UWgtScreenOverlay::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (TimeRemaining > 0.f) {
		TimeRemaining -= DeltaTime;

		if (TimeRemaining <= 0.f)
			SetVisibility(ESlateVisibility::Collapsed);
		else {
			FLinearColor col = ScreenImage->ColorAndOpacity;
			col.A = FMath::Min(TimeRemaining * 2.0f, 1.f) * BaseOpacity;
			ScreenImage->SetColorAndOpacity(col);
		}
	}
}
