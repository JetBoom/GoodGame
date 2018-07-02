#pragma once

#include "Blueprint/UserWidget.h"
#include "WgtBottomBar.generated.h"


UCLASS()
class GOODGAME_API UWgtBottomBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SelectedAbilityText;
	
};
