#pragma once

#include "SchoolData.h"
#include "Ability.h"
#include "AbilityData.h"
#include "StatusEffectData.h"
#include "DamageElementInfo.h"
#include "DataSingleton.generated.h"


USTRUCT()
struct FPlayerBuild
{
	GENERATED_USTRUCT_BODY()

	FPlayerBuild()
	{
		Name = FString("Unknown");
	}

	FPlayerBuild(const FString name, const TArray<uint8> ids)
	{
		Name = name;
		AbilityIDs = ids;
	}

	UPROPERTY(EditAnywhere, Category = PlayerBuild)
	FString Name;

	UPROPERTY(EditAnywhere, Category = PlayerBuild)
	TArray<uint8> AbilityIDs;
};

struct FStatusEffectData;

UCLASS(Blueprintable, BlueprintType)
class UDataSingleton : public UObject
{
	GENERATED_BODY()

public:
	UDataSingleton(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = TransientData)
	void SavePlayerBuilds();
	
	UFUNCTION(BlueprintCallable, Category = TransientData)
	void LoadPlayerBuilds();

	void LoadAbilityData();

	void SaveCameraModes();
	void SaveCameraMode(const uint8& slot);
	void LoadCameraModes();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = Data)
	TArray<FDamageElementInfo> DamageElementTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = Data)
	TArray<FStatusEffectData> StatusEffectTypes;

	UPROPERTY(BlueprintReadWrite, NoClear, Transient, Category = TransientData)
	TArray<FAbilityData> AbilityTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = Data)
	TArray<FSchoolData> SchoolTypes;

	UPROPERTY(BlueprintReadWrite, NoClear, Transient, Category = TransientData)
	TArray<FPlayerBuild> ClientPlayerBuilds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = Data)
	TMap<EAbilityType, FAbilityData> AbilityTypeData;

	UPROPERTY(BlueprintReadWrite, Transient, Category = TransientData)
	TArray<uint8> CameraModeForWeapon;

	//virtual void UDataSingleton::PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

};
