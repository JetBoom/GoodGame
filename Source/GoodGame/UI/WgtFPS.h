#pragma once

#include "Blueprint/UserWidget.h"
#include "TextBlock.h"
#include "WgtFPS.generated.h"


UCLASS()
class GOODGAME_API UWgtFPS : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;

	UPROPERTY(Transient)
	uint16 Frames;

	UPROPERTY(Transient)
	float FrameTime;

	UPROPERTY(Transient)
	int LastFPS = -1;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	
};
