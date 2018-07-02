#pragma once

#include "GameFramework/PlayerController.h"
#include "GoodGamePlayerController.generated.h"


UCLASS()
class GOODGAME_API AGoodGamePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGoodGamePlayerController();

	UFUNCTION(BlueprintCallable, Category = Input)
	void ToggleGameMouse();

	UFUNCTION(BlueprintCallable, Category = Input)
	void ChatEntry();

	UFUNCTION(BlueprintCallable, Category = Input)
	void Escape();

	UPROPERTY(BlueprintReadWrite, Category = Input)
	FVector2D RememberMousePos = FVector2D(0.5f, 0.5f);

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

/*protected:
	void EatInput();
	
	UPROPERTY()
	TArray<FInputActionBinding> InputEaters;*/

};
