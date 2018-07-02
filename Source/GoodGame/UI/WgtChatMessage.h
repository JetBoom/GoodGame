#pragma once

#include "Blueprint/UserWidget.h"
#include "UI/RichTextBox.h"
#include "RespondsToMouseMode.h"
#include "WgtChatMessage.generated.h"


UCLASS()
class GOODGAME_API UWgtChatMessage : public UUserWidget, public IRespondsToMouseMode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UImage* Background;

	UPROPERTY(meta = (BindWidget))
	URichTextBox* TextBox;

	virtual void NativeConstruct() override;
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category = HUD)
	void OnMouseModeSet(const bool mouseEnabled);

};
