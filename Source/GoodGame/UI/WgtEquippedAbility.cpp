#include "GoodGame.h"
#include "CanvasPanelSlot.h"
#include "CanvasPanel.h"
#include "PanelWidget.h"
#include "WidgetTree.h"
#include "WgtEquippedAbility.h"


UWgtEquippedAbility::UWgtEquippedAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	DesignSizeMode = EDesignPreviewSizeMode::CustomOnScreen;
	DesignTimeSize = FVector2D(64, 64);
#endif
}

void UWgtEquippedAbility::NativeConstruct()
{
	Super::NativeConstruct();

	OnEquippedAbilityChanged();
	OnEquippedAbilityCooldownChanged();
}

void UWgtEquippedAbility::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (Cooldown > 0.f) {
		Cooldown = FMath::Max(Cooldown - DeltaTime / CooldownDuration, 0.f);

		OnEquippedAbilityCooldownChanged();
	}
}

FText UWgtEquippedAbility::GetCooldownText() const
{
	const float time = Cooldown * CooldownDuration;

	if (time < 1.f) {
		static const auto fmt = TEXT("%.1f");
		return FText::FromString(FString::Printf(fmt, time));
	}

	//return FText::FromString(FString::FromInt(FMath::CeilToInt(time)));
	return FText::FromString(FString::FromInt(FMath::RoundToInt(time)));
}

void UWgtEquippedAbility::SetAbilityIndex(const uint8& abilityIndex)
{
	EquippedAbilityIndex = abilityIndex;

	OnEquippedAbilityChanged();
	OnEquippedAbilitySelectedChanged();

	RefreshCooldown();
}

void UWgtEquippedAbility::OnEquippedAbilityChanged()
{
	RefreshTickStyle();

	if (!Character)
		return;

	const FAbilityData& data = Character->GetAbilityData(EquippedAbilityIndex);

	if (data.Icon) {
		AbilityImage->SetBrushFromTexture(data.Icon);
		AbilityImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
		AbilityImage->SetVisibility(ESlateVisibility::Hidden);
}

void UWgtEquippedAbility::OnEquippedAbilityCooldownChanged()
{
	RefreshTickStyle();

	static const FName name_clipamount = FName("ClipAmount");
	static const FName name_brightness = FName("Brightness");
	static const FName name_desaturation = FName("Desaturation");

	UMaterialInstanceDynamic* CooldownCircleInstance = AbilityImageOverlay->GetEffectMaterial();
	if (CooldownCircleInstance)
		CooldownCircleInstance->SetScalarParameterValue(name_clipamount, 1.f - Cooldown);

	UMaterialInstanceDynamic* IconInstance = AbilityImageRetainer->GetEffectMaterial();

	if (Cooldown > 0.f) {
		CooldownText->SetText(GetCooldownText());
		CooldownPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
		SelectionRing->SetColorAndOpacity(FLinearColor::Red);

		if (IconInstance) {
			IconInstance->SetScalarParameterValue(name_brightness, 0.9f - Cooldown * 0.4f);
			IconInstance->SetScalarParameterValue(name_desaturation, Cooldown);
		}
	}
	else {
		CooldownPanel->SetVisibility(ESlateVisibility::Hidden);
		SelectionRing->SetColorAndOpacity(FLinearColor::Gray);

		if (IconInstance) {
			IconInstance->SetScalarParameterValue(name_brightness, 1.f);
			IconInstance->SetScalarParameterValue(name_desaturation, 0.f);
		}
	}
}

void UWgtEquippedAbility::OnEquippedAbilitySelectedChanged()
{
	RefreshTickStyle();
}

void UWgtEquippedAbility::RefreshCooldown()
{
	if (Character) {
		const float delta = Character->EquippedAbilities[EquippedAbilityIndex].NextAllowed - WORLD_TIME;
		const float duration = FMath::Max3(Character->GetAbilityData(EquippedAbilityIndex).CoolDown, delta, 0.1f);

		SetCooldown(duration, delta / duration);
	}
	else if (Cooldown > 0.f) {
		Cooldown = 0.f;
		CooldownDuration = 1.f;

		OnEquippedAbilityCooldownChanged();
	}
}

/*AGoodGameCharacter* UWgtEquippedAbility::GetCharacter()
{
	return Cast<AGoodGameCharacter>(GetOwningPlayerPawn());
}*/

void UWgtEquippedAbility::SetCooldown(const float cdDuration, const float cd)
{
	CooldownDuration = cdDuration;
	Cooldown = FMath::Clamp(cd, 0.f, 1.f);

	OnEquippedAbilityCooldownChanged();
}

void UWgtEquippedAbility::RefreshTickStyle()
{
	if (!Character)
		return;

	const bool onCooldown = Cooldown > 0.f;
	const bool selected = Character->SelectedAbility == EquippedAbilityIndex;

	if (onCooldown) {
		if (selected) {
			IndicatorTick->SetBrushFromTexture(TickTextureCooldownSelected);
			IndicatorTick->SetColorAndOpacity(FLinearColor::Red);
		}
		else {
			IndicatorTick->SetBrushFromTexture(TickTexture);
			IndicatorTick->SetColorAndOpacity(FLinearColor::Red);
		}
	}
	else if (selected) {
		IndicatorTick->SetBrushFromTexture(TickTextureSelected);
		IndicatorTick->SetColorAndOpacity(FLinearColor::Yellow);
	}
	else {
		IndicatorTick->SetBrushFromTexture(TickTexture);
		IndicatorTick->SetColorAndOpacity(FLinearColor::Gray);
	}
}
