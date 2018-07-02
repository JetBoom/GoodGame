#include "GoodGame.h"
#include "Weapon.h"
#include "WgtHUDProgressBar.h"

UWgtHUDProgressBar::UWgtHUDProgressBar() {}

void UWgtHUDProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

	SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, bShowing ? 1.f : 0.f));
}

void UWgtHUDProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwningPlayerPawn());

	if (owner) {
		if (owner->CastingAbility)
			Progress = owner->CastingAbility->GetHUDProgress();
		else if (owner->ActiveWeapon)
			Progress = owner->ActiveWeapon->GetHUDProgress();
		else
			Progress = 0.f;
	}
	else
		Progress = 0.f;
	
	const bool show = Progress > 0.f || GetPlayerContext().GetPlayerController()->ShouldShowMouseCursor();
	if (show != bShowing) {
		bShowing = show;
		SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, show ? 1.f : 0.f));
	}
}
