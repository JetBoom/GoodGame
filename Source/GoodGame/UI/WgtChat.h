#pragma once

#include "UI/WgtDraggable.h"
#include "UI/WgtChatMessage.h"
#include "ChatMessageData.h"
#include "PanelWidget.h"
#include "ScrollBox.h"
#include "EditableTextBox.h"
#include "HorizontalBox.h"
#include "RespondsToMouseMode.h"
#include "WgtChat.generated.h"


UCLASS()
class GOODGAME_API UWgtChat : public UWgtDraggable, public IRespondsToMouseMode
{
	GENERATED_BODY()
	
public:
	const static int MaxChatMessages = 40;

	UWgtChat(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Chat)
	virtual void AddChatMessage(const FChatMessage& message);

	UFUNCTION(BlueprintCallable, Category = Chat)
	void FocusChatEntry() { ChatInput->SetKeyboardFocus(); }

	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ChatMessages;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ChatInput;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* WindowHeader;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* BottomArea;

	UPROPERTY(meta = (BindWidget))
	UImage* ShadowTop;

	UPROPERTY(meta = (BindWidget))
	UImage* ShadowBottom;

	UPROPERTY(meta = (BindWidget))
	UImage* Background;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Widgets)
	TAssetSubclassOf<UWgtChatMessage> ChatMessageClass;

	UFUNCTION(BlueprintNativeEvent, Category = HUD)
	void OnMouseModeSet(const bool mouseEnabled);

	UFUNCTION()
	virtual void OnChatTextCommitted(const FText& InText, ETextCommit::Type InCommitType);

};
