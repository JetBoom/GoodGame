#include "GoodGame.h"
#include "UI/RichTextBox.h"
#include "GoodGamePlayerState.h"
#include "WgtChat.h"


void UWgtChat::NativeConstruct()
{
	//ChatInput->AddBinding(ChatInput->OnTextCommitted, this, &UWgtChat::OnChatTextCommitted);
	ChatInput->OnTextCommitted.AddDynamic(this, &UWgtChat::OnChatTextCommitted);
}

UWgtChat::UWgtChat(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UClass> C_ChatMessage(TEXT("/Game/UI/WgtChatMessage_BP.WgtChatMessage_BP_C"));
	ChatMessageClass = C_ChatMessage.Object;
}

void UWgtChat::OnChatTextCommitted(const FText& InText, ETextCommit::Type InCommitType)
{
	//UE_LOG(LogTemp, Log, TEXT("Pressed enter"));
	if (InCommitType == ETextCommit::OnEnter && !ChatInput->Text.IsEmptyOrWhitespace()) {
		auto ggps = (AGoodGamePlayerState*)GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerState;
		ggps->ClientToServerChatMessage(FChatMessage(InText.ToString(), EChatMessageType::Global));

		ChatInput->SetText(FText::GetEmpty());
	}
}

void UWgtChat::AddChatMessage(const FChatMessage& message)
{
	if (!ChatMessageClass)
		return;

	auto wgt = WidgetTree->ConstructWidget<UWgtChatMessage>(ChatMessageClass.LoadSynchronous());
	if (!wgt)
		return;

	FString text = message.Message;
	FString prefix;
	if (message.TypeID == (uint8)EChatMessageType::Private) {
		prefix = "[PM] ";
		wgt->ColorAndOpacity = FLinearColor(0.7f, 0.f, 0.7f);
	}
	else if (message.TypeID == (uint8)EChatMessageType::Global) {
		//prefix = "[Global] ";
		wgt->ColorAndOpacity = FLinearColor(0.72f, 0.66f, 0.42f);
	}
	else if (message.TypeID == (uint8)EChatMessageType::Area) {
		prefix = "[Area] ";
		wgt->ColorAndOpacity = FLinearColor(0.7f, 0.7f, 0.7f);
	}

	const auto ps = Cast<AGoodGamePlayerState>(message.Actor);
	if (ps)
		prefix += "<span style=\"bold\">[" + ps->PlayerName + "]</>: ";

	text = prefix + text;

	wgt->TextBox->SetText(FText::FromString(text));

	//const bool atend = ChatMessages->GetScrollOffset() <= 0.f;

	ChatMessages->AddChild(wgt);

	UE_LOG(LogTemp, Log, TEXT("Chat: %s"), *text);

	if (ChatMessages->GetChildrenCount() > MaxChatMessages)
		ChatMessages->RemoveChildAt(1); // Index 0 is held by a dummy widget

	//if (atend || ChatMessages->ScrollBarVisibility == ESlateVisibility::Collapsed)
		ChatMessages->ScrollToEnd();
}

void UWgtChat::OnMouseModeSet_Implementation(const bool mouseEnabled)
{
	const auto vis = mouseEnabled ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;

	WindowHeader->SetVisibility(vis);
	BottomArea->SetVisibility(vis);

	// Cosmetic
	ShadowTop->SetVisibility(vis);
	ShadowBottom->SetVisibility(vis);
	Background->SetVisibility(vis);

	if (mouseEnabled) {
		ChatMessages->ScrollBarVisibility = ESlateVisibility::Visible;
		
		ChatMessages->WidgetBarStyle.NormalThumbImage.DrawAs = ESlateBrushDrawType::Box;

		ChatMessages->WidgetBarStyle.VerticalBottomSlotImage.DrawAs =
		ChatMessages->WidgetBarStyle.VerticalTopSlotImage.DrawAs = ESlateBrushDrawType::Image;
	}
	else {
		ChatMessages->ScrollBarVisibility = ESlateVisibility::Collapsed;

		ChatMessages->WidgetBarStyle.NormalThumbImage.DrawAs =
		ChatMessages->WidgetBarStyle.VerticalBottomSlotImage.DrawAs = 
		ChatMessages->WidgetBarStyle.VerticalTopSlotImage.DrawAs = ESlateBrushDrawType::NoDrawType;

		ChatMessages->ScrollToEnd();
	}

	UWgtChatMessage* cm = nullptr;
	for (int i = 1; i < ChatMessages->GetChildrenCount(); i++) {
		cm = (UWgtChatMessage*)ChatMessages->GetChildAt(i);
		if (cm)
			cm->Execute_OnMouseModeSet(cm, mouseEnabled);
	}
}
