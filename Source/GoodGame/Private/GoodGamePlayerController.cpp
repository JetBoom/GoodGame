#include "GoodGame.h"
#include "GoodGameHUD.h"
#include "GoodGamePlayerController.h"

AGoodGamePlayerController::AGoodGamePlayerController() {}

void AGoodGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Internal_ToggleGameMouse", IE_Pressed, this, &AGoodGamePlayerController::ToggleGameMouse);
	InputComponent->BindAction("Internal_ChatEntry", IE_Pressed, this, &AGoodGamePlayerController::ChatEntry);
	InputComponent->BindAction("Internal_Escape", IE_Pressed, this, &AGoodGamePlayerController::Escape);
}

void AGoodGamePlayerController::ChatEntry()
{
	if (!bShowMouseCursor) {
		ToggleGameMouse();

		auto HUD = Cast<AGoodGameHUD>(GetHUD());
		if (HUD)
			HUD->Chat->FocusChatEntry();
	}
}

void AGoodGamePlayerController::Escape()
{
	if (bShowMouseCursor)
		ToggleGameMouse();
}

void AGoodGamePlayerController::ToggleGameMouse()
{
	if (bShowMouseCursor) {
		RememberMousePos;

		float x;
		float y;
		if (GetMousePosition(x, y)) {
			int vx;
			int vy;
			GetViewportSize(vx, vy);

			RememberMousePos.X = FMath::Clamp(x / vx, 0.f, 1.f);
			RememberMousePos.Y = FMath::Clamp(y / vy, 0.f, 1.f);
		}

		bShowMouseCursor = false;
		ResetIgnoreLookInput();
		ResetIgnoreMoveInput();
		InputComponent->bBlockInput = false;

		static const auto nput = FInputModeGameOnly();
		SetInputMode(nput);
	}
	else {
		bShowMouseCursor = true;
		SetIgnoreLookInput(true);
		SetIgnoreMoveInput(true);
		InputComponent->bBlockInput = true;

		static const auto nput = FInputModeGameAndUI(); //FInputModeUIOnly();
		SetInputMode(nput);

		int vx;
		int vy;
		GetViewportSize(vx, vy);

		SetMouseLocation(RememberMousePos.X * vx, RememberMousePos.Y * vy);
	}

	/*for (auto& eater : InputEaters)
		eater.bConsumeInput = bShowMouseCursor;*/
		
	// Let HUD know so it can change some widget states.
	auto HUD = Cast<AGoodGameHUD>(GetHUD());
	if (HUD)
		HUD->OnMouseStateChanged(bShowMouseCursor);
}

void AGoodGamePlayerController::BeginPlay()
{
	Super::BeginPlay();
}
