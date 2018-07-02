#include "GoodGame.h"
#include "GoodGamePlayerController.h"
#include "WgtChatMessage.h"


void UWgtChatMessage::NativeConstruct()
{
	Super::NativeConstruct();

	const auto controller = Cast<AGoodGamePlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (controller)
		OnMouseModeSet_Implementation(controller->bShowMouseCursor);
}

void UWgtChatMessage::OnMouseModeSet_Implementation(const bool mouseEnabled)
{
	Background->SetVisibility(mouseEnabled ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible);
}
