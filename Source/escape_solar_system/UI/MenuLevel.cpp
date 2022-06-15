// Copyright 2020 H₂S. All Rights Reserved.


#include "MenuLevel.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include "MainFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "MenuLevel"

const int Max_Level = 4;

void UMenuLevelHelper::GetRestorationHP(TArray<UItemDataObject*>& OutItems)
{
	for (UBackpackComponent* Bp : GetBackpackList())
	{
		const TMultiMap<FName, int32>& ItemList = Bp->GetItemList();
		for (const auto& Elem : ItemList)
		{
			if (FCString::Atoi(*Elem.Key.ToString()) > 0)
			{
				UItemDataObject* Obj = NewObject<UItemDataObject>(this);
				Obj->RowName = Elem.Key;
				Obj->Count = Elem.Value;
				OutItems.Add(Obj);
			}
		}
	}
}

void UMenuLevelHelper::GetRestorationMP(TArray<UItemDataObject*>& OutItems)
{
	for (UBackpackComponent* Bp : GetBackpackList())
	{
		const TMultiMap<FName, int32>& ItemList = Bp->GetItemList();
		for (const auto& Elem : ItemList)
		{
			if (FCString::Atoi(*Elem.Key.ToString()) > 0)
			{
				UItemDataObject* Obj = NewObject<UItemDataObject>(this);
				Obj->RowName = Elem.Key;
				Obj->Count = Elem.Value;
				OutItems.Add(Obj);
			}
		}
	}
}

PRAGMA_DISABLE_OPTIMIZATION
void UMenuLevelHelper::SelectLevel(const TArray<ELevel>& Prop)
{
	CurLevelInfo = FText();
	NextLevelInfo = FText();
	DemandPoints = FText();
	DemandItems = FText();

	ELevel Level = Prop[0];
	int Val = *GetTarget(Level);
	FString InfoStr;
	if (Val >= 0)
	{
		for (const auto& elemt : Prop) {
			InfoStr += LV::DispName[elemt].ToString() + TEXT(": ");
			InfoStr += FString::FormatAsNumber(UMainFunctionLibrary::GetLevelValue(elemt, Val)) + TEXT("\n");
		}
		InfoStr.RemoveFromEnd(TEXT("\n"));
		CurLevelInfo = FText::FromString(InfoStr);
	}
	if (Val < Max_Level)
	{
		InfoStr = FString();
		for (const auto& elemt : Prop) {
			InfoStr += LV::DispName[elemt].ToString() + TEXT(": ");
			InfoStr += FString::FormatAsNumber(UMainFunctionLibrary::GetLevelValue(elemt, Val+1)) + TEXT("\n");
		}
		InfoStr.RemoveFromEnd(TEXT("\n"));
		NextLevelInfo = FText::FromString(InfoStr);

		FLevelDemand LevelDemand = UMainFunctionLibrary::GetLevelDemand(Level, Val+1);
		DemandPoints = FText::Format(LOCTEXT("Points", "探索点数: {0}"), LevelDemand.Points);

		AEarthBase* EarthBase = AMainLevelScriptActor::GetMainChar()->FindEarthBase();
		UBackpackComponent* Backpack = EarthBase ? EarthBase->Backpack : nullptr;
		auto DemandInfo = UMainFunctionLibrary::GetDemandInfo(LevelDemand.Items, Backpack);
		const int32 Mock_Points = 999;
		CanUpgrade = DemandInfo.Key && Mock_Points>=LevelDemand.Points;
		DemandItems = DemandInfo.Value;
	}
	else
	{
		CanUpgrade = false;
	}
}
PRAGMA_ENABLE_OPTIMIZATION

void UMenuLevelHelper::UpgradeLevel(const TArray<ELevel>& Prop)
{
	if (!CanUpgrade || !AMainLevelScriptActor::GetEarthBase()) return;

	ELevel Level = Prop[0];
	int* ValPtr = GetTarget(Level);
	if (*ValPtr < Max_Level)
	{
		FLevelDemand LevelDemand = UMainFunctionLibrary::GetLevelDemand(Level, *ValPtr);
		UBackpackComponent* Backpack = AMainLevelScriptActor::GetEarthBase()->Backpack;
		for (const auto& Demand : LevelDemand.Items)
		{
			Backpack->RemoveItem(Demand.Key, Demand.Value);
		}
		*ValPtr += 1;
		AMainLevelScriptActor::GetMainChar()->ResetProperties();
		if (AMainLevelScriptActor::GetSpaceship()) {
			AMainLevelScriptActor::GetSpaceship()->ResetProperties();
		}
		SelectLevel(Prop);
	}
}

int32 UMenuLevelHelper::GetMaxVal(const TArray<ELevel>& Prop)
{
	// 暂时默认最大等级都是5
	return 5;
}

int32 UMenuLevelHelper::GetCurVal(const TArray<ELevel>& Prop)
{
	return *GetTarget(Prop[0]) + 1;
}

void UMenuLevelHelper::Debug()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	AMainCharacter* Char = AMainLevelScriptActor::GetMainChar();
	Char->Body->ChangeHP(Char->Body->GetMaximumHP());
	Char->Engine->ChangeEnergy(Char->Engine->GetMaximumEnergy());

	ASpaceship* Ship = Char->FindSpaceship();
	if (Ship)
	{
		Ship->Body->ChangeHP(Ship->Body->GetMaximumHP());
		Ship->Engine->ChangeEnergy(Ship->Engine->GetMaximumEnergy());
	}
#endif
}

int* UMenuLevelHelper::GetTarget(ELevel Level)
{
	AMainCharacter* Char = AMainLevelScriptActor::GetMainChar();
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
	case ELevel::CharShieldCold:
		ValPtr = &Char->LevelShieldCold;
		break;
	case ELevel::CharShieldHeat:
		ValPtr = &Char->LevelShieldHeat;
		break;
	case ELevel::CharShieldPress:
		ValPtr = &Char->LevelShieldPress;
		break;
	case ELevel::CharEnginePower:
	case ELevel::CharEngineMass:
	case ELevel::CharEngineEPR:
	case ELevel::CharEngineEMR:
		ValPtr = &Char->LevelEngine;
		break;
	case ELevel::CharEnergy:
		ValPtr = &Char->LevelEnergy;
		break;
	case ELevel::ShipHP:
	case ELevel::ShipMass:
		check(Ship);
		ValPtr = &Ship->LevelStrength;
		break;
	case ELevel::ShipBackpack:
		check(Ship);
		ValPtr = &Ship->LevelBackpack;
		break;
	case ELevel::ShipShieldCold:
		check(Ship);
		ValPtr = &Ship->LevelShieldCold;
		break;
	case ELevel::ShipShieldHeat:
		check(Ship);
		ValPtr = &Ship->LevelShieldHeat;
		break;
	case ELevel::ShipShieldPress:
		check(Ship);
		ValPtr = &Ship->LevelShieldPress;
		break;
	case ELevel::ShipEngine0Power:
	case ELevel::ShipEngine0Mass:
	case ELevel::ShipEngine0EPR:
	case ELevel::ShipEngine0EMR:
		check(Ship);
		ValPtr = &Ship->LevelEngine0;
		break;
	case ELevel::ShipEngine1Power:
	case ELevel::ShipEngine1Mass:
	case ELevel::ShipEngine1EPR:
	case ELevel::ShipEngine1EMR:
		check(Ship);
		ValPtr = &Ship->LevelEngine1;
		break;
	case ELevel::ShipEngine2Power:
	case ELevel::ShipEngine2Mass:
	case ELevel::ShipEngine2EPR:
	case ELevel::ShipEngine2EMR:
		check(Ship);
		ValPtr = &Ship->LevelEngine2;
		break;
	case ELevel::ShipEnergy0:
		check(Ship);
		ValPtr = &Ship->LevelEnergy0;
		break;
	case ELevel::ShipEnergy1:
		check(Ship);
		ValPtr = &Ship->LevelEnergy1;
		break;
	case ELevel::ShipEnergy2:
		check(Ship);
		ValPtr = &Ship->LevelEnergy2;
		break;
	}

	check(ValPtr);
	return ValPtr;
}

inline TArray<class UBackpackComponent*> UMenuLevelHelper::GetBackpackList()
{
	TArray<UBackpackComponent*> BpList;
	AMainCharacter* Char = AMainLevelScriptActor::GetMainChar();
	BpList.Add(Char->Backpack);
	if (Char->FindSpaceship())
	{
		BpList.Add(Char->FindSpaceship()->Backpack);
	}
	if (Char->FindEarthBase())
	{
		BpList.Add(Char->FindEarthBase()->Backpack);
	}
	return BpList;
}

#undef LOCTEXT_NAMESPACE
