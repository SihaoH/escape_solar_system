// Copyright 2020 H₂S. All Rights Reserved.


#include "MenuLevel.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include "MainFunctionLibrary.h"
#include "MainPlayerState.h"

#define LOCTEXT_NAMESPACE "MenuLevel"

const int Max_Level = 4;

void UMenuLevelHelper::SelectLevel(const TArray<ELevel>& Props)
{
	CurLevelInfo = FText();
	NextLevelInfo = FText();
	DemandPoints = FText();
	DemandItems = FText();

	ELevel Level = Props[0];
	int Val = *GetTarget(Level);
	FString InfoStr;
	if (Val >= 0)
	{
		for (const auto& elemt : Props) {
			InfoStr += LV::DispName[elemt].ToString() + TEXT(": ");
			InfoStr += FString::FormatAsNumber(UMainFunctionLibrary::GetLevelValue(elemt, Val)) + TEXT("\n");
		}
		InfoStr.RemoveFromEnd(TEXT("\n"));
		CurLevelInfo = FText::FromString(InfoStr);
	}
	if (Val < Max_Level)
	{
		auto PlayerState = AMainPlayerState::Instance();
		InfoStr = FString();
		for (const auto& elemt : Props) {
			InfoStr += LV::DispName[elemt].ToString() + TEXT(": ");
			InfoStr += FString::FormatAsNumber(UMainFunctionLibrary::GetLevelValue(elemt, Val+1)) + TEXT("\n");
		}
		InfoStr.RemoveFromEnd(TEXT("\n"));
		NextLevelInfo = FText::FromString(InfoStr);

		FLevelDemand LevelDemand = UMainFunctionLibrary::GetLevelDemand(Level, Val+1);
		// 已升级过的技能不消耗探索点
		DemandPoints = FText::Format(LOCTEXT("Points", "探索点数: {0}"), Val >= PlayerState->GetBestLevel(Props) ? LevelDemand.Points : 0);

		AEarthBase* EarthBase = AMainLevelScriptActor::GetMainChar()->FindEarthBase();
		UBackpackComponent* Backpack = EarthBase ? EarthBase->Backpack : nullptr;
		auto DemandInfo = UMainFunctionLibrary::GetDemandInfo(LevelDemand.Items, Backpack);
		CanUpgrade = DemandInfo.Key && (PlayerState->GetExplorePoints() >= LevelDemand.Points);
		DemandItems = DemandInfo.Value;
	}
	else
	{
		CanUpgrade = false;
	}
}

void UMenuLevelHelper::UpgradeLevel(const TArray<ELevel>& Props)
{
	if (!CanUpgrade || !AMainLevelScriptActor::GetEarthBase()) return;

	ELevel Level = Props[0];
	int* ValPtr = GetTarget(Level);
	if (*ValPtr < Max_Level)
	{
		FLevelDemand LevelDemand = UMainFunctionLibrary::GetLevelDemand(Level, *ValPtr+1);
		auto PlayerState = AMainPlayerState::Instance();
		auto Backpack = AMainLevelScriptActor::GetEarthBase()->Backpack;
		// 已升级过的技能不消耗探索点
		if (*ValPtr >= PlayerState->GetBestLevel(Props))
		{
			PlayerState->ChangeExplorePoints(-LevelDemand.Points);
		}
		for (const auto& Demand : LevelDemand.Items)
		{
			Backpack->RemoveItem(Demand.Key, Demand.Value);
		}
		*ValPtr += 1;
		AMainLevelScriptActor::GetMainChar()->ResetProperties();
		if (AMainLevelScriptActor::GetSpaceship()) {
			AMainLevelScriptActor::GetSpaceship()->ResetProperties();
		}
		SelectLevel(Props);
	}
}

FText UMenuLevelHelper::GetLevelName(const TArray<ELevel>& Props)
{
	ELevel Level = Props[0];
	FText Name;
	switch (Level)
	{
	case ELevel::CharHP:
	case ELevel::CharMass:
		Name = LOCTEXT("CharHP", "机身强度");
		break;
	case ELevel::CharBackpack:
		Name = LOCTEXT("CharBackpack", "背包");
		break;
	case ELevel::CharShieldCold:
		Name = LOCTEXT("CharShieldCold", "冷防护");
		break;
	case ELevel::CharShieldHeat:
		Name = LOCTEXT("CharShieldHeat", "热防护");
		break;
	case ELevel::CharShieldPress:
		Name = LOCTEXT("CharShieldPress", "压力防护");
		break;
	case ELevel::CharEnginePower:
	case ELevel::CharEngineMass:
	case ELevel::CharEngineEPR:
	case ELevel::CharEngineEMR:
		Name = LOCTEXT("CharEnginePower", "化学引擎");
		break;
	case ELevel::CharEnergy:
		Name = LOCTEXT("CharEnergy", "化学燃料仓");
		break;
	case ELevel::ShipHP:
	case ELevel::ShipMass:
		Name = LOCTEXT("ShipHP", "机身强度");
		break;
	case ELevel::ShipBackpack:
		Name = LOCTEXT("ShipBackpack", "存储仓");
		break;
	case ELevel::ShipShieldCold:
		Name = LOCTEXT("ShipShieldCold", "冷防护");
		break;
	case ELevel::ShipShieldHeat:
		Name = LOCTEXT("ShipShieldHeat", "热防护");
		break;
	case ELevel::ShipShieldPress:
		Name = LOCTEXT("ShipShieldPress", "压力防护");
		break;
	case ELevel::ShipEngine0Power:
	case ELevel::ShipEngine0Mass:
	case ELevel::ShipEngine0EPR:
	case ELevel::ShipEngine0EMR:
		Name = LOCTEXT("ShipEngine0Power", "化学引擎");
		break;
	case ELevel::ShipEngine1Power:
	case ELevel::ShipEngine1Mass:
	case ELevel::ShipEngine1EPR:
	case ELevel::ShipEngine1EMR:
		Name = LOCTEXT("ShipEngine1Power", "核裂变引擎");
		break;
	case ELevel::ShipEngine2Power:
	case ELevel::ShipEngine2Mass:
	case ELevel::ShipEngine2EPR:
	case ELevel::ShipEngine2EMR:
		Name = LOCTEXT("ShipEngine2Power", "核聚变引擎");
		break;
	case ELevel::ShipEnergy0:
		Name = LOCTEXT("ShipEnergy0", "化学燃料仓");
		break;
	case ELevel::ShipEnergy1:
		Name = LOCTEXT("ShipEnergy1", "核裂变燃料仓");
		break;
	case ELevel::ShipEnergy2:
		Name = LOCTEXT("ShipEnergy2", "核聚变燃料仓");
		break;
	}

	return Name;
}

FText UMenuLevelHelper::GetLevelDesc(const TArray<ELevel>& Props)
{
	ELevel Level = Props[0];
	FText Desc;
	switch (Level)
	{
	case ELevel::CharHP:
	case ELevel::CharMass:
		Desc = LOCTEXT("CharHP", "机身强度");
		break;
	case ELevel::CharBackpack:
		Desc = LOCTEXT("CharBackpack", "背包");
		break;
	case ELevel::CharShieldCold:
		Desc = LOCTEXT("CharShieldCold", "冷防护");
		break;
	case ELevel::CharShieldHeat:
		Desc = LOCTEXT("CharShieldHeat", "热防护");
		break;
	case ELevel::CharShieldPress:
		Desc = LOCTEXT("CharShieldPress", "压力防护");
		break;
	case ELevel::CharEnginePower:
	case ELevel::CharEngineMass:
	case ELevel::CharEngineEPR:
	case ELevel::CharEngineEMR:
		Desc = LOCTEXT("CharEnginePower", "化学引擎");
		break;
	case ELevel::CharEnergy:
		Desc = LOCTEXT("CharEnergy", "化学燃料仓");
		break;
	case ELevel::ShipHP:
	case ELevel::ShipMass:
		Desc = LOCTEXT("ShipHP", "机身强度");
		break;
	case ELevel::ShipBackpack:
		Desc = LOCTEXT("ShipBackpack", "存储仓");
		break;
	case ELevel::ShipShieldCold:
		Desc = LOCTEXT("ShipShieldCold", "冷防护");
		break;
	case ELevel::ShipShieldHeat:
		Desc = LOCTEXT("ShipShieldHeat", "热防护");
		break;
	case ELevel::ShipShieldPress:
		Desc = LOCTEXT("ShipShieldPress", "压力防护");
		break;
	case ELevel::ShipEngine0Power:
	case ELevel::ShipEngine0Mass:
	case ELevel::ShipEngine0EPR:
	case ELevel::ShipEngine0EMR:
		Desc = LOCTEXT("ShipEngine0Power", "化学引擎");
		break;
	case ELevel::ShipEngine1Power:
	case ELevel::ShipEngine1Mass:
	case ELevel::ShipEngine1EPR:
	case ELevel::ShipEngine1EMR:
		Desc = LOCTEXT("ShipEngine1Power", "核裂变引擎");
		break;
	case ELevel::ShipEngine2Power:
	case ELevel::ShipEngine2Mass:
	case ELevel::ShipEngine2EPR:
	case ELevel::ShipEngine2EMR:
		Desc = LOCTEXT("ShipEngine2Power", "核聚变引擎");
		break;
	case ELevel::ShipEnergy0:
		Desc = LOCTEXT("ShipEnergy0", "化学燃料仓");
		break;
	case ELevel::ShipEnergy1:
		Desc = LOCTEXT("ShipEnergy1", "核裂变燃料仓");
		break;
	case ELevel::ShipEnergy2:
		Desc = LOCTEXT("ShipEnergy2", "核聚变燃料仓");
		break;
	}

	return Desc;
}

int32 UMenuLevelHelper::GetMaxVal(const TArray<ELevel>& Props)
{
	// 暂时默认最大等级都是5
	return 5;
}

int32 UMenuLevelHelper::GetCurVal(const TArray<ELevel>& Props)
{
	return *GetTarget(Props[0]) + 1;
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
