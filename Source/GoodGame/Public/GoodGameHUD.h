#pragma once

#include "GameFramework/HUD.h"
#include "UI/WgtHealthBar.h"
#include "UI/WgtHUDProgressBar.h"
#include "UI/WgtEquippedAbility.h"
#include "UI/WgtChat.h"
#include "UI/WgtTargetID.h"
#include "UI/WgtTargetHealthBar.h"
#include "UI/Wgt3DNumberFloater.h"
#include "UI/WgtHotbars.h"
#include "UI/WgtBottomBar.h"
#include "UI/WgtScreenOverlay.h"
#include "ChatMessageData.h"
#include "GoodGameHUD.generated.h"


UCLASS()
class GOODGAME_API AGoodGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGoodGameHUD();

	UFUNCTION(BlueprintCallable, Category = Chat)
	void AddChatMessage(const FChatMessage& message);

	/*UFUNCTION(BlueprintCallable, Category = HUD)
	void SetGameMouseMode(const bool mouseEnabled);

	UFUNCTION(BlueprintCallable, Category = HUD)
	void ToggleGameMouseMode();*/

	UFUNCTION(BlueprintCallable, Category = HUD)
	void OnMouseStateChanged(const bool& mouseEnabled);

	UFUNCTION(Exec, Category = ConsoleCommands)
	void PrintDistance();

	UFUNCTION(BlueprintCallable, Category = HUD)
	void AddDamageFloater(AActor* target, const float& amount, const bool& isCritical = false, const bool& isHeal = false);

	void RefreshSelectedAbility();

	TAssetSubclassOf<UWgtDraggable> PersonalHealthBarWidgetClass;
	TAssetSubclassOf<UWgtDraggable> PersonalManaBarWidgetClass;
	TAssetSubclassOf<UWgtDraggable> PersonalStaminaBarWidgetClass;
	TAssetSubclassOf<UUserWidget> CrosshairWidgetClass;
	TAssetSubclassOf<UUserWidget> TopBarWidgetClass;
	TAssetSubclassOf<UWgtBottomBar> BottomBarWidgetClass;
	TAssetSubclassOf<UWgtHUDProgressBar> HUDProgressBarWidgetClass;
	TAssetSubclassOf<UWgtEquippedAbility> HUDEquippedAbilityWidgetClass;
	TAssetSubclassOf<UWgtChat> ChatClass;
	TAssetSubclassOf<UWgtTargetID> TargetIDClass;
	TAssetSubclassOf<UWgtTargetHealthBar> TargetHealthBarClass;
	TAssetSubclassOf<UWgt3DNumberFloater> NumberFloaterClass;
	TAssetSubclassOf<UWgtHotbars> HotbarsClass;
	TAssetSubclassOf<UWgtScreenOverlay> PummelOverlayClass;
	TAssetSubclassOf<UWgtScreenOverlay> BloodOverlayClass;

	UPROPERTY(BlueprintReadWrite, Category = Widgets)
	UWgtDraggable* PersonalHealthBarWidget;

	UPROPERTY(BlueprintReadWrite, Category = Widgets)
	UWgtDraggable* PersonalManaBarWidget;

	UPROPERTY(BlueprintReadWrite, Category = Widgets)
	UWgtDraggable* PersonalStaminaBarWidget;

	UPROPERTY(BlueprintReadWrite, Category = Widgets)
	UUserWidget* CrosshairWidget;

	UPROPERTY(BlueprintReadWrite, Category = Widgets)
	UUserWidget* TopBarWidget;

	UPROPERTY(BlueprintReadWrite, Category = Widgets)
	UWgtBottomBar* BottomBarWidget;

	UPROPERTY(BlueprintReadWrite, Category = Widgets)
	UWgtHUDProgressBar* HUDProgressBarWidget;

	UPROPERTY(BlueprintReadWrite, Category = Widgets)
	TArray<UWgtEquippedAbility*> EquippedAbilityWidgets;

	UPROPERTY(BlueprintReadOnly, Category = Widgets)
	UWgtChat* Chat;

	UPROPERTY(BlueprintReadOnly, Category = Widgets)
	UWgtTargetID* TargetID;

	UPROPERTY(BlueprintReadOnly, Category = Widgets)
	UWgtTargetHealthBar* TargetHealthBar;

	UPROPERTY(BlueprintReadOnly, Category = Widgets)
	UWgtHotbars* Hotbars;

	UPROPERTY(BlueprintReadOnly, Category = Widgets)
	UWgtScreenOverlay* PummelOverlay;

	UPROPERTY(BlueprintReadOnly, Category = Widgets)
	UWgtScreenOverlay* BloodOverlay;

	UPROPERTY(BlueprintReadWrite, Category = HUD)
	bool bMouseEnabled;

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	FHitResult CurrentTargetID;

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	TArray<UWgt3DNumberFloater*> NumberFloaters;

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	TArray<UWgt3DNumberFloater*> NumberFloaterPool;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(Transient)
	APawn* CurrentPawn;

	virtual void DrawHUD() override;

};
