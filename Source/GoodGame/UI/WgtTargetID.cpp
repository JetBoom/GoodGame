#include "GoodGame.h"
#include "HasTargetInfo.h"
#include "WgtTargetID.h"


void UWgtTargetID::UpdateTargetID(const AActor* target)
{
	const auto targetinfo = Cast<IHasTargetInfo>(target);
	if (targetinfo) {
		NameText->SetText(FText::FromString(targetinfo->GetTargetName()));

		const FString subname = targetinfo->GetTargetSubName();

		if (subname.IsEmpty())
			SubWidget->SetVisibility(ESlateVisibility::Collapsed);
		else {
			SubText->SetText(FText::FromString(subname));

			SubWidget->SetVisibility(ESlateVisibility::Visible);
		}

		SetVisibility(ESlateVisibility::Visible);
	}
	else
		SetVisibility(ESlateVisibility::Collapsed);
}
