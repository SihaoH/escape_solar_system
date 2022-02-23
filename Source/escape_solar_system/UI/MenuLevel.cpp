// Copyright 2020 H₂S. All Rights Reserved.


#include "MenuLevel.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include "MainFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "MenuLevel"

const int Max_Level = 4;

void UMenuLevel::SelectLevel(const TArray<ELevel>& Prop)
{
	ELevel Level = Prop[0];
	int Val = *GetTarget(Level);
	FString InfoStr;
	for (const auto& elemt : Prop) {
		InfoStr += LV::DispName[elemt].ToString() + TEXT(": ");
		InfoStr += FString::FormatAsNumber(UMainFunctionLibrary::GetLevelValue(elemt, Val)) + TEXT("\n");
	}
	InfoStr.RemoveFromEnd(TEXT("\n"));
	CurLevelInfo = FText::FromString(InfoStr);

	NextLevelInfo = FText();
	DemandPoints = FText();
	DemandItems = FText();
	if (Val < Max_Level)
	{
		InfoStr = FString();
		for (const auto& elemt : Prop) {
			InfoStr += LV::DispName[elemt].ToString() + TEXT(": ");
			InfoStr += FString::FormatAsNumber(UMainFunctionLibrary::GetLevelValue(elemt, Val+1)) + TEXT("\n");
		}
		InfoStr.RemoveFromEnd(TEXT("\n"));
		NextLevelInfo = FText::FromString(InfoStr);

		FLevelDemand LevelDemand = UMainFunctionLibrary::GetLevelDemand(Level, Val);
		DemandPoints = FText::Format(LOCTEXT("Points", "探索点数: {0}"), LevelDemand.Points);

		AEarthBaseActor* EarthBase = AMainCharacter::GetInstance()->FindEarthBase();
		UBackpackComponent* Backpack = EarthBase ? EarthBase->GetBackpack() : nullptr;
		auto DemandInfo = UMainFunctionLibrary::GetDemandInfo(LevelDemand.Items, Backpack);
		const int32 Mock_Points = 999;
		CanUpgrade = DemandInfo.Key && Mock_Points>=LevelDemand.Points;
		DemandItems = DemandInfo.Value;
	}
}

void UMenuLevel::UpgradeLevel(const TArray<ELevel>& Prop)
{
	check(CanUpgrade);
	AMainCharacter* Char = AMainCharacter::GetInstance();
	ELevel Level = Prop[0];
	int* ValPtr = GetTarget(Level);
	if (*ValPtr < Max_Level)
	{
		FLevelDemand LevelDemand = UMainFunctionLibrary::GetLevelDemand(Level, *ValPtr);
		UBackpackComponent* Backpack = AMainCharacter::GetInstance()->FindEarthBase()->GetBackpack();
		for (const auto& Demand : LevelDemand.Items)
		{
			Backpack->RemoveItem(Demand.Key, Demand.Value);
		}
		*ValPtr += 1;
		Char->ResetProperties();
	}
}

int32 UMenuLevel::GetMaxVal(const TArray<ELevel>& Prop)
{
	// 暂时默认最大等级都是5
	return 5;
}

int32 UMenuLevel::GetCurVal(const TArray<ELevel>& Prop)
{
	return *GetTarget(Prop[0]) + 1;
}

int* UMenuLevel::GetTarget(ELevel Level)
{
	AMainCharacter* Char = AMainCharacter::GetInstance();
	ASpaceship* Ship = Char->FindSpaceship();

	int* ValPtr = nullptr;
	switch (Level)
	{
	case ELevel::CharHP:
	case ELevel::CharMass:
		ValPtr = &Char->LevelStrength;
		break;
	case ELevel::CharBackpack:
		ValPtr = &Char->LevelBackpack;
		break;
	}

	check(ValPtr);
	return ValPtr;
}

#undef LOCTEXT_NAMESPACE
