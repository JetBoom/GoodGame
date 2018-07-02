#pragma once

#include "Blueprint/UserWidget.h"
#include "UI/WgtDraggable.h"
#include "Image.h"
#include "TextBlock.h"
#include "WgtTargetHealthBar.generated.h"


UCLASS()
class GOODGAME_API UWgtTargetHealthBar : public UWgtDraggable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = HealthBar)
	void SetHealthPercentage(uint8 percentage);

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* Bar;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;

	UPROPERTY(Transient)
	uint8 LastPercentage = 100;
	
};
