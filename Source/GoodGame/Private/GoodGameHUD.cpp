#include "GoodGame.h"
#include "GoodGamePlayerController.h"
#include "GoodGameHUD.h"


AGoodGameHUD::AGoodGameHUD()
{
	static ConstructorHelpers::FObjectFinder<UClass> C_PersonalHealthBar(TEXT("/Game/UI/WgtPersonalHealthBar_BP.WgtPersonalHealthBar_BP_C"));
	PersonalHealthBarWidgetClass = C_PersonalHealthBar.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_PersonalManaBar(TEXT("/Game/UI/WgtPersonalManaBar_BP.WgtPersonalManaBar_BP_C"));
	PersonalManaBarWidgetClass = C_PersonalManaBar.Object;
	
	static ConstructorHelpers::FObjectFinder<UClass> C_PersonalStaminaBar(TEXT("/Game/UI/WgtPersonalStaminaBar_BP.WgtPersonalStaminaBar_BP_C"));
	PersonalStaminaBarWidgetClass = C_PersonalStaminaBar.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_Crosshair(TEXT("/Game/UI/WgtCrosshair_BP.WgtCrosshair_BP_C"));
	CrosshairWidgetClass = C_Crosshair.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_TopBar(TEXT("/Game/UI/WgtTopBar_BP.WgtTopBar_BP_C"));
	TopBarWidgetClass = C_TopBar.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_BottomBar(TEXT("/Game/UI/WgtBottomBar_BP.WgtBottomBar_BP_C"));
	BottomBarWidgetClass = C_BottomBar.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_HUDProgressBar(TEXT("/Game/UI/WgtHUDProgressBar_BP.WgtHUDProgressBar_BP_C"));
	HUDProgressBarWidgetClass = C_HUDProgressBar.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_HUDEquippedAbility(TEXT("/Game/UI/WgtEquippedAbility_BP.WgtEquippedAbility_BP_C"));
	HUDEquippedAbilityWidgetClass = C_HUDEquippedAbility.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_Chat(TEXT("/Game/UI/WgtChat_BP.WgtChat_BP_C"));
	ChatClass = C_Chat.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_TargetID(TEXT("/Game/UI/WgtTargetID_BP.WgtTargetID_BP_C"));
	TargetIDClass = C_TargetID.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_TargetHealthBar(TEXT("/Game/UI/WgtTargetHealthBar_BP.WgtTargetHealthBar_BP_C"));
	TargetHealthBarClass = C_TargetHealthBar.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_NumberFloater(TEXT("/Game/UI/Wgt3DNumberFloater_BP.Wgt3DNumberFloater_BP_C"));
	NumberFloaterClass = C_NumberFloater.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_Hotbars(TEXT("/Game/UI/WgtHotbars_BP.WgtHotbars_BP_C"));
	HotbarsClass = C_Hotbars.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_PummelOverlay(TEXT("/Game/UI/WgtOverlayPummel_BP.WgtOverlayPummel_BP_C"));
	PummelOverlayClass = C_PummelOverlay.Object;

	static ConstructorHelpers::FObjectFinder<UClass> C_BloodOverlay(TEXT("/Game/UI/WgtOverlayBlood_BP.WgtOverlayBlood_BP_C"));
	BloodOverlayClass = C_BloodOverlay.Object;
}

void AGoodGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AGoodGameHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!PlayerOwner)
		return;

	TArray<UWgt3DNumberFloater*> toRemove;
	for (UWgt3DNumberFloater* floater : NumberFloaters) {
		floater->Time += DeltaSeconds;

		if (floater->Time < floater->LifeTime) {
			AActor* floaterActor = floater->Actor;

			if (floaterActor && floaterActor->IsValidLowLevel()) {
				floater->SetFloaterAlpha(FMath::Pow(1.f - floater->Time / floater->LifeTime, 0.5f));
				if (!floater->bNoRaise)
					floater->Offset.Z += DeltaSeconds * 50.f / floater->LifeTime;

				FVector2D out_ScreenLocation;
				PlayerOwner->ProjectWorldLocationToScreen(floaterActor->GetActorLocation() + floater->Offset, out_ScreenLocation);

				floater->SetPositionInViewport(out_ScreenLocation);

				continue;
			}
		}

		toRemove.Add(floater);
	}

	// Return expired floaters to the pool.
	for (UWgt3DNumberFloater* floater : toRemove) {
		NumberFloaterPool.Add(floater);
		NumberFloaters.RemoveSingle(floater);
		floater->SetVisibility(ESlateVisibility::Collapsed);
	}

	APawn* pawn = PlayerOwner->GetPawn();

	if (pawn != CurrentPawn) {
		CurrentPawn = pawn;

		AGoodGameCharacter* character = Cast<AGoodGameCharacter>(pawn);
		for (UWgtEquippedAbility* equipped : EquippedAbilityWidgets) {
			equipped->Character = character;
			equipped->OnEquippedAbilityChanged();
		}

		RefreshSelectedAbility();
	}

	if (pawn) {
		const APlayerController* controller = Cast<APlayerController>(pawn->Controller);
		if (controller) {
			FVector pos;
			FRotator rot;
			controller->PlayerCameraManager->GetCameraViewPoint(pos, rot);

			FCollisionQueryParams params;
			params.AddIgnoredActor(pawn);

			GetWorld()->LineTraceSingleByChannel(CurrentTargetID, pos, pos + rot.Vector() * 7000.f, ECollisionChannel::ECC_Visibility, params);

			if (TargetID) {
				const AActor* actor = CurrentTargetID.Actor.Get();
				TargetID->UpdateTargetID(actor);

				const IHasHealth* hashealth = Cast<IHasHealth>(actor);
				if (hashealth) {
					TargetHealthBar->SetHealthPercentage(FMath::RoundToInt(hashealth->GetHealth() / hashealth->GetMaxHealth() * 100.f));
					TargetHealthBar->SetVisibility(ESlateVisibility::Visible);
				}
				else
					TargetHealthBar->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void AGoodGameHUD::RefreshSelectedAbility()
{
	const AGoodGameCharacter* character = Cast<AGoodGameCharacter>(CurrentPawn);
	if (!character)
		return;

	BottomBarWidget->SelectedAbilityText->SetText(FText::FromString(character->GetAbilityData(character->SelectedAbility).Name));
}

void AGoodGameHUD::PrintDistance()
{
	const FString str = "Distance: " + FString::FromInt((int)CurrentTargetID.Distance);

	AddChatMessage(FChatMessage(str, EChatMessageType::System));
	BottomBarWidget->SelectedAbilityText->SetText(FText::FromString(str));
}

void AGoodGameHUD::BeginPlay()
{
	Super::BeginPlay();

	auto parent = GetGameInstance();

	TargetID = CreateWidget<UWgtTargetID>(parent, TargetIDClass.LoadSynchronous());
	TargetID->AddToViewport();
	TargetID->SetDefaultAnchorsInViewport(FVector2D(0.5f, 0.6f));
	TargetID->SetDefaultAlignmentInViewport(FVector2D(0.5f, 0.f));

	TargetHealthBar = CreateWidget<UWgtTargetHealthBar>(parent, TargetHealthBarClass.LoadSynchronous());
	TargetHealthBar->AddToViewport();
	TargetHealthBar->SetDefaultAnchorsInViewport(FVector2D(0.5f, 0.6f));
	TargetHealthBar->SetDefaultAlignmentInViewport(FVector2D(0.5f, 1.f));

	PummelOverlay = CreateWidget<UWgtScreenOverlay>(parent, PummelOverlayClass.LoadSynchronous());
	PummelOverlay->AddToViewport();

	BloodOverlay = CreateWidget<UWgtScreenOverlay>(parent, BloodOverlayClass.LoadSynchronous());
	BloodOverlay->AddToViewport();

	Hotbars = CreateWidget<UWgtHotbars>(parent, HotbarsClass.LoadSynchronous());
	Hotbars->AddToViewport();
	Hotbars->SetAnchorsInViewport(FAnchors(0.5f, 1.f));
	Hotbars->SetAlignmentInViewport(FVector2D(0.5f, 1.f));

	EquippedAbilityWidgets.Reserve(AAbility::PlayerAbilitySlots);
	EquippedAbilityWidgets.SetNum(AAbility::PlayerAbilitySlots);
	for (uint8 i = 0; i < AAbility::PlayerAbilitySlots; ++i) {
		UWgtEquippedAbility* wgt = CreateWidget<UWgtEquippedAbility>(parent, HUDEquippedAbilityWidgetClass.LoadSynchronous());
		//wgt->SetOwningLocalPlayer((ULocalPlayer*)GetNetOwningPlayer());
		EquippedAbilityWidgets[i] = wgt;

		wgt->SetAbilityIndex(i);

		Hotbars->AddWidgetToBar(wgt);
	}

	PersonalHealthBarWidget = CreateWidget<UWgtDraggable>(parent, PersonalHealthBarWidgetClass.LoadSynchronous());
	PersonalHealthBarWidget->AddToViewport();
	PersonalHealthBarWidget->SetDefaultAnchorsInViewport(FVector2D(0.5f, 0.75f));
	PersonalHealthBarWidget->SetDefaultAlignmentInViewport(FVector2D(0.5f, 0.f));

	PersonalStaminaBarWidget = CreateWidget<UWgtDraggable>(parent, PersonalStaminaBarWidgetClass.LoadSynchronous());
	PersonalStaminaBarWidget->AddToViewport();
	PersonalStaminaBarWidget->SetDefaultAnchorsInViewport(FVector2D(0.5f, 0.78f));
	PersonalStaminaBarWidget->SetDefaultAlignmentInViewport(FVector2D(0.5f, 0.f));

	PersonalManaBarWidget = CreateWidget<UWgtDraggable>(parent, PersonalManaBarWidgetClass.LoadSynchronous());
	PersonalManaBarWidget->AddToViewport();
	PersonalManaBarWidget->SetDefaultAnchorsInViewport(FVector2D(0.5f, 0.81f));
	PersonalManaBarWidget->SetDefaultAlignmentInViewport(FVector2D(0.5f, 0.f));

	CrosshairWidget = CreateWidget<UUserWidget>(parent, CrosshairWidgetClass.LoadSynchronous());
	CrosshairWidget->AddToViewport();
	CrosshairWidget->SetAnchorsInViewport(FAnchors(0.5f, 0.5f));
	CrosshairWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

	HUDProgressBarWidget = CreateWidget<UWgtHUDProgressBar>(parent, HUDProgressBarWidgetClass.LoadSynchronous());
	HUDProgressBarWidget->AddToViewport();
	HUDProgressBarWidget->SetDefaultAnchorsInViewport(FVector2D(0.5f, 0.25f));
	HUDProgressBarWidget->SetDefaultAlignmentInViewport(FVector2D(0.5f, 0.5f));

	Chat = CreateWidget<UWgtChat>(parent, ChatClass.LoadSynchronous());
	Chat->AddToViewport();
	Chat->SetDefaultAnchorsInViewport(FVector2D(0.f, 0.75f));
	Chat->SetDefaultAlignmentInViewport(FVector2D(0.f, 0.f));

	NumberFloaterPool.Reserve(32);
	NumberFloaterPool.SetNum(32);
	UWgt3DNumberFloater* floater = nullptr;
	for (uint8 i = 0; i < 32; ++i) {
		floater = CreateWidget<UWgt3DNumberFloater>(parent, NumberFloaterClass.LoadSynchronous());

		floater->AddToViewport();
		floater->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
		floater->SetVisibility(ESlateVisibility::Collapsed);

		NumberFloaterPool[i] = floater;
	}

	TopBarWidget = CreateWidget<UUserWidget>(parent, TopBarWidgetClass.LoadSynchronous());
	TopBarWidget->AddToViewport();

	BottomBarWidget = CreateWidget<UWgtBottomBar>(parent, BottomBarWidgetClass.LoadSynchronous());
	BottomBarWidget->AddToViewport();

	auto controller = Cast<AGoodGamePlayerController>(GetOwningPlayerController());
	if (controller)
		OnMouseStateChanged(controller->bShowMouseCursor);
}

void AGoodGameHUD::AddDamageFloater(AActor* target, const float& amount, const bool& isCritical, const bool& isHeal)
{
	bool recycle = false;
	UWgt3DNumberFloater* floater = nullptr;

	if (NumberFloaterPool.Num() == 0) {
		floater = NumberFloaters[0];
		recycle = true;
	}
	else
		floater = NumberFloaterPool.Pop(true);

	if (floater) {
		FVector out_Extent;

		if (target->IsA<APawn>() && ((APawn*)target)->IsLocallyControlled()) {
			out_Extent = FMath::VRand() * FMath::RandRange(6.f, 24.f);
			out_Extent.Z += 32.f;

			//floater->bNoRaise = true;
		}
		else {
			FVector out_Origin;
			target->GetActorBounds(true, out_Origin, out_Extent);

			out_Extent.X = FMath::RandRange(-out_Extent.X, out_Extent.X) / 4.f;
			out_Extent.Y = FMath::RandRange(-out_Extent.Y, out_Extent.Y) / 4.f;
			//out_Extent.Z = out_Extent.Z / 2.f;

			//floater->bNoRaise = false;
		}

		floater->Actor = target;
		floater->Offset = out_Extent;
		floater->InitFloater(amount, isCritical, isHeal);

		if (!recycle)
			NumberFloaters.Add(floater);
	}
}

void AGoodGameHUD::AddChatMessage(const FChatMessage& message)
{
	if (Chat)
		Chat->AddChatMessage(message);
}

/*void AGoodGameHUD::ToggleGameMouseMode()
{
	SetGameMouseMode(!bMouseEnabled);
}*/

void AGoodGameHUD::OnMouseStateChanged(const bool& mouseEnabled)
{
	//if (bMouseEnabled == mouseEnabled)
	//	return;

	bMouseEnabled = mouseEnabled;

	Chat->Execute_OnMouseModeSet(Chat, bMouseEnabled);
}
