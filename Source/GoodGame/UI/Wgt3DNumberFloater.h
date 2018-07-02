#pragma once

#include "Blueprint/UserWidget.h"
#include "Wgt3DNumberFloater.generated.h"


UCLASS()
class GOODGAME_API UWgt3DNumberFloater : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitFloater(const float& amount, const bool& isCritical = false, const bool& isHeal = false);

	void SetFloaterAlpha(const float& alpha);

	AActor* Actor;
	FVector Offset;
	float Time;
	bool bNoRaise;

	float LifeTime = 1.25f;

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSub;
	
};
