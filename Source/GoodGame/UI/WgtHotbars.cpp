#include "GoodGame.h"
#include "WgtHotbars.h"


void UWgtHotbars::AddWidgetToBar(UUserWidget* widget)
{
	widget->RemoveFromParent();

	//if (LeftBar->GetChildrenCount() < AAbility::PlayerAbilitySlots / 2)
	if (LeftBar->GetChildrenCount() < AAbility::PlayerCommonAbilities)
		LeftBar->AddChild(widget);
	else
		RightBar->AddChild(widget);

	((UHorizontalBoxSlot*)widget->Slot)->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
}
