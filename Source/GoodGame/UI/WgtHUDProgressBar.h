#pragma once

#include "UI/WgtDraggable.h"
#include "WgtHUDProgressBar.generated.h"


UCLASS()
class GOODGAME_API UWgtHUDProgressBar : public UWgtDraggable
{
	GENERATED_BODY()
	
public:
	UWgtHUDProgressBar();

	// What progress is showing? 0 will make the widget invisible.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Progress)
	float Progress = 0.f;

	UPROPERTY(Transient, BlueprintReadWrite, Category = Progress)
	uint32 bShowing:1;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
