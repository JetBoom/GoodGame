#pragma once

#include "Blueprint/UserWidget.h"
#include "WgtScreenOverlay.generated.h"


UCLASS()
class GOODGAME_API UWgtScreenOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BeginOverlay(const float& time);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ScreenImage;

	UPROPERTY(EditAnywhere, Category = Overlay)
	float BaseOpacity = 1.f;

	float TimeRemaining;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

};
