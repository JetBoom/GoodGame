#pragma once

#include "Blueprint/UserWidget.h"
#include "HorizontalBox.h"
#include "WgtHotbars.generated.h"


UCLASS()
class GOODGAME_API UWgtHotbars : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddWidgetToBar(UUserWidget* widget);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* LeftBar;
	
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* RightBar;
	
};
