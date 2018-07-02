#pragma once

#include "Blueprint/UserWidget.h"
#include "TextBlock.h"
#include "WgtPing.generated.h"


UCLASS()
class GOODGAME_API UWgtPing : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;

	UPROPERTY(Transient)
	float UpdateTimeLeft;

	UPROPERTY(Transient)
	int LastPing = -1;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

};
