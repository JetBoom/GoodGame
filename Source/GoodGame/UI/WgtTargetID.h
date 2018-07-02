#pragma once

#include "Blueprint/UserWidget.h"
#include "UI/WgtDraggable.h"
#include "TextBlock.h"
#include "WgtTargetID.generated.h"


UCLASS()
class GOODGAME_API UWgtTargetID : public UWgtDraggable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = TargetID)
	void UpdateTargetID(const AActor* target);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SubText;

	UPROPERTY(meta = (BindWidget))
	UWidget* SubWidget;

};
