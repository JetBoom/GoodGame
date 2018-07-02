#include "GoodGame.h"
#include "DataSingleton.h"


UDataSingleton::UDataSingleton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CameraModeForWeapon.SetNum((int)EWeaponSlots::LENGTH + 1);
	CameraModeForWeapon[0] = 0;
	CameraModeForWeapon[(int)EWeaponSlots::Melee] = 0;
	CameraModeForWeapon[(int)EWeaponSlots::Bow] = 1;
	CameraModeForWeapon[(int)EWeaponSlots::Staff] = 1;
	CameraModeForWeapon[(int)EWeaponSlots::Other] = 0;
	CameraModeForWeapon[(int)EWeaponSlots::LENGTH] = 0;
}

void UDataSingleton::SaveCameraModes()
{
	if (CameraModeForWeapon.Num() < (int)EWeaponSlots::LENGTH + 1)
		return;

	static const auto section = FString("CameraModeForWeapons");

	for (int i = 0; i < CameraModeForWeapon.Num(); ++i) {
		const auto key = FString("W") + FString::FromInt(i);
		GConfig->SetInt(*section, *key, CameraModeForWeapon[i], GGameIni);
	}

	GConfig->Flush(false, GGameIni);
}

void UDataSingleton::SaveCameraMode(const uint8& slot)
{
	if (slot >= CameraModeForWeapon.Num())
		return;

	static const auto section = FString("CameraModeForWeapons");
	const auto key = FString("W") + FString::FromInt(slot);

	GConfig->SetInt(*section, *key, CameraModeForWeapon[slot], GGameIni);
	GConfig->Flush(false, GGameIni);

	//UE_LOG(LogTemp, Log, TEXT("Save slot %d to %d"), slot, CameraModeForWeapon[slot]);
}

void UDataSingleton::LoadCameraModes()
{
	static const auto section = FString("CameraModeForWeapons");

	int val;

	for (int i = 0; i <= (int)EWeaponSlots::LENGTH; ++i) {
		const auto key = FString("W") + FString::FromInt(i);
		if (GConfig->GetInt(*section, *key, val, GGameIni)) {
			if (val < 0 || val > 3)
				val = 0;

			CameraModeForWeapon[i] = val;
		}
	}
}

void UDataSingleton::LoadAbilityData()
{
	AbilityTypes.SetNum(AbilityTypeData.Num(), true);
	for (auto& abilityTypePair : AbilityTypeData) {
		FAbilityData& abilityType = abilityTypePair.Value;
		abilityType.AbilityTypeID = abilityTypePair.Key;
		AbilityTypes[(uint8)abilityTypePair.Key] = abilityType;
	}

	// Setup shared cooldowns. Making sure all data has references to all others.
	for (const FAbilityData& abilityType : AbilityTypes) {
		for (const EAbilityType& otherSharedID : abilityType.SharedCooldowns)
			AbilityTypes[(int)otherSharedID].SharedCooldowns.AddUnique(abilityType.AbilityTypeID);
	}
}

/*void UDataSingleton::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	//FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	//if (PropertyName == GET_MEMBER_NAME_CHECKED(UDataSingleton, AbilityTypeData)) {
		for (auto a : AbilityTypes) {
			if (!AbilityTypeData.Contains(a.AbilityTypeID)) {
				AbilityTypeData.Add(a.AbilityTypeID, a);
			}
		}
	//}
}*/

void UDataSingleton::SavePlayerBuilds()
{
	if (!GConfig)
		return;

	static const auto TEXT_NAME = TEXT("Name");
	static const auto TEXT_ABILITYIDS = TEXT("AbilityIDs");

	const uint8 numbuilds = ClientPlayerBuilds.Num();

	for (uint8 i = 0; i < MAX_PLAYER_BUILDS; ++i) {
		auto section = FString::Printf(TEXT("PlayerBuild%d"), i);

		if (i >= numbuilds) {
			GConfig->EmptySection(*section, GGameIni);
		}
		else {
			TArray<FString> stringabilityids;
			for (uint8 j = 0; j < ClientPlayerBuilds[i].AbilityIDs.Num(); j++)
				stringabilityids[j] = FString::FromInt(ClientPlayerBuilds[i].AbilityIDs[j]);

			GConfig->SetString(*section, TEXT_NAME, *ClientPlayerBuilds[i].Name, GGameIni);
			GConfig->SetArray(*section, TEXT_ABILITYIDS, stringabilityids, GGameIni);
		}
	}

	GConfig->Flush(false, GGameIni);
}

void UDataSingleton::LoadPlayerBuilds()
{
	if (!GConfig) {
		UE_LOG(LogTemp, Log, TEXT("No config."));
		return;
	}

	static const auto TEXT_NAME = TEXT("Name");
	static const auto TEXT_ABILITYIDS = TEXT("AbilityIDs");

	FString out_Name;
	TArray<FString> out_AbilityIDs;

	ClientPlayerBuilds.Empty();

	for (uint8 i = 0; i < MAX_PLAYER_BUILDS; ++i) {
		auto section = FString::Printf(TEXT("PlayerBuild%d"), i);

		if (GConfig->DoesSectionExist(*section, GGameIni) && GConfig->GetString(*section, TEXT_NAME, out_Name, GGameIni) && GConfig->GetArray(*section, TEXT_ABILITYIDS, out_AbilityIDs, GGameIni)) {
			TArray<uint8> abilityids;
			abilityids.SetNum(out_AbilityIDs.Num(), false);

			for (uint8 j = 0; j < out_AbilityIDs.Num(); j++)
				abilityids[j] = (uint8)FCString::Atoi(*out_AbilityIDs[j]);

			ClientPlayerBuilds.Add(FPlayerBuild(out_Name, abilityids));
		}
	}
}
