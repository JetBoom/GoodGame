#pragma once

#include "Blueprint/UserWidget.h"
#include "RetainerBox.h"
#include "WgtEquippedAbility.generated.h"


UCLASS()
class GOODGAME_API UWgtEquippedAbility : public UUserWidget
{
	GENERATED_BODY()

public:
	UWgtEquippedAbility(const FObjectInitializer& ObjectInitializer);

	void OnEquippedAbilityChanged();

	void OnEquippedAbilityCooldownChanged();

	void OnEquippedAbilitySelectedChanged();

	UFUNCTION(BlueprintCallable, Category = EquippedAbility)
	void SetAbilityIndex(UPARAM(ref) const uint8& abilityIndex);
	//void SetCharacterAndIndex(AGoodGameCharacter* character, const uint8 abilityIndex);

	UFUNCTION(BlueprintCallable, Category = EquippedAbility)
	void SetCooldown(const float cdDuration, const float cd = 1.f);

	UFUNCTION(BlueprintCallable, Category = EquippedAbility)
	void RefreshCooldown();

	UFUNCTION(BlueprintCallable, Category = EquippedAbility)
	FText GetCooldownText() const;

	AGoodGameCharacter* Character;

	/*UFUNCTION(BlueprintCallable, Category = EquippedAbility)
	AGoodGameCharacter* GetCharacter();*/
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EquippedAbility, meta = (AllowPrivateAccess = "true"))
	uint8 EquippedAbilityIndex;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EquippedAbility, meta = (AllowPrivateAccess = "true"))
	//AGoodGameCharacter* Character;

	// Number between [0, 1]. Decremented on tick.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EquippedAbility, meta = (AllowPrivateAccess = "true"))
	float Cooldown;

	// How long the total cooldown is.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EquippedAbility, meta = (AllowPrivateAccess = "true"))
	float CooldownDuration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EquippedAbility, meta = (AllowPrivateAccess = "true"))
	bool bIsSelected;

	UPROPERTY(EditAnywhere, Category = EquippedAbility)
	UTexture2D* TickTexture;

	UPROPERTY(EditAnywhere, Category = EquippedAbility)
	UTexture2D* TickTextureSelected;

	UPROPERTY(EditAnywhere, Category = EquippedAbility)
	UTexture2D* TickTextureCooldownSelected;

	UPROPERTY(EditAnywhere, Category = EquippedAbility)
	UTexture2D* DefaultAbilityTexture;

	UPROPERTY(meta = (BindWidget))
	UImage* IndicatorTick;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooldownText;

	UPROPERTY(meta = (BindWidget))
	UWidget* CooldownPanel;

	UPROPERTY(meta = (BindWidget))
	UImage* AbilityImage;

	UPROPERTY(meta = (BindWidget))
	UImage* SelectionRing;

	UPROPERTY(meta = (BindWidget))
	URetainerBox* AbilityImageOverlay;

	UPROPERTY(meta = (BindWidget))
	URetainerBox* AbilityImageRetainer;

	void RefreshTickStyle();

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	
};
