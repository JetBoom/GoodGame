#include "GoodGame.h"
#include "Wgt3DNumberFloater.h"


void UWgt3DNumberFloater::InitFloater(const float& amount, const bool& isCritical, const bool& isHeal)
{
	const APawn* pawn = Cast<APawn>(Actor);
	const bool isLocal = pawn && pawn->IsLocallyControlled();

	if (isHeal) {
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		TextSub->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	}
	else if (isLocal) {
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		TextSub->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	}
	else if (isCritical) {
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		TextSub->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	}
	else {
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TextSub->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}

	int whole = FMath::FloorToInt(amount);
	int dec = FMath::FloorToInt((amount - whole) * 10.f);

	if (dec > 0) {
		TextSub->SetText(FText::FromString(FString(".") + FString::FromInt(dec)));
		TextSub->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
		TextSub->SetVisibility(ESlateVisibility::Collapsed);

	if (isHeal)
		Text->SetText(FText::FromString("+" + FString::FromInt(whole)));
	else if (isLocal)
		Text->SetText(FText::FromString("-" + FString::FromInt(whole)));
	else
		Text->SetText(FText::FromString(FString::FromInt(whole)));

	Time = 0.f;
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UWgt3DNumberFloater::SetFloaterAlpha(const float& alpha)
{
	SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, alpha));
}
