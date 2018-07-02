#pragma once

#include "UI/WgtDraggable.h"
#include "Image.h"
#include "TextBlock.h"
#include "WgtStaminaBar.generated.h"


UCLASS()
class GOODGAME_API UWgtStaminaBar : public UWgtDraggable
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* Bar;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;

	UPROPERTY(Transient)
	int LastStamina = -1;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	
};
