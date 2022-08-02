﻿// Copyright 2020 H₂S. All Rights Reserved.


#include "MenuTech.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include "MainLibrary.h"
#include "MainPlayerState.h"

#define LOCTEXT_NAMESPACE "MenuLevel"

const int Max_Level = 4;

void UMenuTechHelper::SelectTech(const TArray<ETech>& Props)
{
	CurLevelInfo = FText();
	NextLevelInfo = FText();
	DemandPoints = FText();
	DemandItems = FText();

	ETech Level = Props[0];
	int Val = *GetTarget(Level);
	FString InfoStr;
	if (Val >= 0)
	{
		for (const auto& elemt : Props) {
			InfoStr += TECH::DispName[elemt].ToString() + TEXT(": ");
			InfoStr += FString::FormatAsNumber(UMainLibrary::GetTechValue(elemt, Val)) + TEXT("\n");
		}
		InfoStr.RemoveFromEnd(TEXT("\n"));
		CurLevelInfo = FText::FromString(InfoStr);
	}
	if (Val < Max_Level)
	{
		auto PlayerState = AMainPlayerState::Instance();
		InfoStr = FString();
		for (const auto& elemt : Props) {
			InfoStr += TECH::DispName[elemt].ToString() + TEXT(": ");
			InfoStr += FString::FormatAsNumber(UMainLibrary::GetTechValue(elemt, Val+1)) + TEXT("\n");
		}
		InfoStr.RemoveFromEnd(TEXT("\n"));
		NextLevelInfo = FText::FromString(InfoStr);

		FTechDemand LevelDemand = UMainLibrary::GetTechDemand(Level, Val+1);
		// 已升级过的技能不消耗探索点
		DemandPoints = FText::Format(LOCTEXT("Points", "探索点数: {0}"), Val >= PlayerState->GetBestLevel(Props) ? LevelDemand.Points : 0);

		AEarthBase* EarthBase = AMainLevelScript::GetMainChar()->FindEarthBase();
		UBackpackComponent* Backpack = EarthBase ? EarthBase->Backpack : nullptr;
		auto DemandInfo = UMainLibrary::GetDemandInfo(LevelDemand.Items, Backpack);
		CanUpgrade = DemandInfo.Key && (PlayerState->GetExplorePoints() >= LevelDemand.Points);
		DemandItems = DemandInfo.Value;
	}
	else
	{
		CanUpgrade = false;
	}
}

void UMenuTechHelper::UpgradeTech(const TArray<ETech>& Props)
{
	if (!CanUpgrade || !AMainLevelScript::GetEarthBase()) return;

	ETech Level = Props[0];
	int* ValPtr = GetTarget(Level);
	if (*ValPtr < Max_Level)
	{
		FTechDemand LevelDemand = UMainLibrary::GetTechDemand(Level, *ValPtr+1);
		auto PlayerState = AMainPlayerState::Instance();
		auto Backpack = AMainLevelScript::GetEarthBase()->Backpack;
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
		AMainLevelScript::GetMainChar()->ResetProperties();
		if (AMainLevelScript::GetSpaceship()) {
			AMainLevelScript::GetSpaceship()->ResetProperties();
		}
		SelectTech(Props);
	}
}

FText UMenuTechHelper::GetTechName(const TArray<ETech>& Props)
{
	ETech Level = Props[0];
	FText Name;
	switch (Level)
	{
	case ETech::CharHP:
	case ETech::CharMass:
		Name = LOCTEXT("CharHP", "机身强度");
		break;
	case ETech::CharBackpack:
		Name = LOCTEXT("CharBackpack", "背包");
		break;
	case ETech::CharShieldCold:
		Name = LOCTEXT("CharShieldCold", "冷防护");
		break;
	case ETech::CharShieldHeat:
		Name = LOCTEXT("CharShieldHeat", "热防护");
		break;
	case ETech::CharShieldPress:
		Name = LOCTEXT("CharShieldPress", "压力防护");
		break;
	case ETech::CharEnginePower:
	case ETech::CharEngineMass:
	case ETech::CharEngineEPR:
	case ETech::CharEngineEMR:
		Name = LOCTEXT("CharEnginePower", "化学引擎");
		break;
	case ETech::CharEnergy:
		Name = LOCTEXT("CharEnergy", "化学燃料仓");
		break;
	case ETech::ShipHP:
	case ETech::ShipMass:
		Name = LOCTEXT("ShipHP", "机身强度");
		break;
	case ETech::ShipBackpack:
		Name = LOCTEXT("ShipBackpack", "存储仓");
		break;
	case ETech::ShipShieldCold:
		Name = LOCTEXT("ShipShieldCold", "冷防护");
		break;
	case ETech::ShipShieldHeat:
		Name = LOCTEXT("ShipShieldHeat", "热防护");
		break;
	case ETech::ShipShieldPress:
		Name = LOCTEXT("ShipShieldPress", "压力防护");
		break;
	case ETech::ShipEngine0Power:
	case ETech::ShipEngine0Mass:
	case ETech::ShipEngine0EPR:
	case ETech::ShipEngine0EMR:
		Name = LOCTEXT("ShipEngine0Power", "化学引擎");
		break;
	case ETech::ShipEngine1Power:
	case ETech::ShipEngine1Mass:
	case ETech::ShipEngine1EPR:
	case ETech::ShipEngine1EMR:
		Name = LOCTEXT("ShipEngine1Power", "核裂变引擎");
		break;
	case ETech::ShipEngine2Power:
	case ETech::ShipEngine2Mass:
	case ETech::ShipEngine2EPR:
	case ETech::ShipEngine2EMR:
		Name = LOCTEXT("ShipEngine2Power", "核聚变引擎");
		break;
	case ETech::ShipEnergy0:
		Name = LOCTEXT("ShipEnergy0", "化学燃料仓");
		break;
	case ETech::ShipEnergy1:
		Name = LOCTEXT("ShipEnergy1", "核裂变燃料仓");
		break;
	case ETech::ShipEnergy2:
		Name = LOCTEXT("ShipEnergy2", "核聚变燃料仓");
		break;
	}

	return Name;
}

FText UMenuTechHelper::GetTechDesc(const TArray<ETech>& Props)
{
	ETech Level = Props[0];
	FText Desc;
	switch (Level)
	{
	case ETech::CharHP:
	case ETech::CharMass:
		Desc = LOCTEXT("CharHP", "机身强度");
		break;
	case ETech::CharBackpack:
		Desc = LOCTEXT("CharBackpack", "背包");
		break;
	case ETech::CharShieldCold:
		Desc = LOCTEXT("CharShieldCold", "冷防护");
		break;
	case ETech::CharShieldHeat:
		Desc = LOCTEXT("CharShieldHeat", "热防护");
		break;
	case ETech::CharShieldPress:
		Desc = LOCTEXT("CharShieldPress", "压力防护");
		break;
	case ETech::CharEnginePower:
	case ETech::CharEngineMass:
	case ETech::CharEngineEPR:
	case ETech::CharEngineEMR:
		Desc = LOCTEXT("CharEnginePower", "化学引擎");
		break;
	case ETech::CharEnergy:
		Desc = LOCTEXT("CharEnergy", "化学燃料仓");
		break;
	case ETech::ShipHP:
	case ETech::ShipMass:
		Desc = LOCTEXT("ShipHP", "机身强度");
		break;
	case ETech::ShipBackpack:
		Desc = LOCTEXT("ShipBackpack", "存储仓");
		break;
	case ETech::ShipShieldCold:
		Desc = LOCTEXT("ShipShieldCold", "冷防护");
		break;
	case ETech::ShipShieldHeat:
		Desc = LOCTEXT("ShipShieldHeat", "热防护");
		break;
	case ETech::ShipShieldPress:
		Desc = LOCTEXT("ShipShieldPress", "压力防护");
		break;
	case ETech::ShipEngine0Power:
	case ETech::ShipEngine0Mass:
	case ETech::ShipEngine0EPR:
	case ETech::ShipEngine0EMR:
		Desc = LOCTEXT("ShipEngine0Power", "化学引擎");
		break;
	case ETech::ShipEngine1Power:
	case ETech::ShipEngine1Mass:
	case ETech::ShipEngine1EPR:
	case ETech::ShipEngine1EMR:
		Desc = LOCTEXT("ShipEngine1Power", "核裂变引擎");
		break;
	case ETech::ShipEngine2Power:
	case ETech::ShipEngine2Mass:
	case ETech::ShipEngine2EPR:
	case ETech::ShipEngine2EMR:
		Desc = LOCTEXT("ShipEngine2Power", "核聚变引擎");
		break;
	case ETech::ShipEnergy0:
		Desc = LOCTEXT("ShipEnergy0", "化学燃料仓");
		break;
	case ETech::ShipEnergy1:
		Desc = LOCTEXT("ShipEnergy1", "核裂变燃料仓");
		break;
	case ETech::ShipEnergy2:
		Desc = LOCTEXT("ShipEnergy2", "核聚变燃料仓");
		break;
	}

	return Desc;
}

int32 UMenuTechHelper::GetMaxLv(const TArray<ETech>& Props)
{
	// 暂时默认最大等级都是5
	return 5;
}

int32 UMenuTechHelper::GetCurLv(const TArray<ETech>& Props)
{
	return *GetTarget(Props[0]) + 1;
}

void UMenuTechHelper::Debug()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	auto Char = AMainLevelScript::GetMainChar();
	Char->Body->ChangeHP(Char->Body->GetMaximumHP());
	Char->Engine->ChangeEnergy(Char->Engine->GetMaximumEnergy());

	auto Ship = AMainLevelScript::GetSpaceship();
	if (Ship)
	{
		Ship->Body->ChangeHP(Ship->Body->GetMaximumHP());
		Ship->Engine->ChangeEnergy(Ship->Engine->GetMaximumEnergy());
	}
#endif
}

int* UMenuTechHelper::GetTarget(ETech Level)
{
	auto Char = AMainLevelScript::GetMainChar();
	auto Ship = AMainLevelScript::GetSpaceship();

	int* ValPtr = nullptr;
	switch (Level)
	{
	case ETech::CharHP:
	case ETech::CharMass:
		ValPtr = &Char->LevelStrength;
		break;
	case ETech::CharBackpack:
		ValPtr = &Char->LevelBackpack;
		break;
	case ETech::CharShieldCold:
		ValPtr = &Char->LevelShieldCold;
		break;
	case ETech::CharShieldHeat:
		ValPtr = &Char->LevelShieldHeat;
		break;
	case ETech::CharShieldPress:
		ValPtr = &Char->LevelShieldPress;
		break;
	case ETech::CharEnginePower:
	case ETech::CharEngineMass:
	case ETech::CharEngineEPR:
	case ETech::CharEngineEMR:
		ValPtr = &Char->LevelEngine;
		break;
	case ETech::CharEnergy:
		ValPtr = &Char->LevelEnergy;
		break;
	case ETech::ShipHP:
	case ETech::ShipMass:
		check(Ship);
		ValPtr = &Ship->LevelStrength;
		break;
	case ETech::ShipBackpack:
		check(Ship);
		ValPtr = &Ship->LevelBackpack;
		break;
	case ETech::ShipShieldCold:
		check(Ship);
		ValPtr = &Ship->LevelShieldCold;
		break;
	case ETech::ShipShieldHeat:
		check(Ship);
		ValPtr = &Ship->LevelShieldHeat;
		break;
	case ETech::ShipShieldPress:
		check(Ship);
		ValPtr = &Ship->LevelShieldPress;
		break;
	case ETech::ShipEngine0Power:
	case ETech::ShipEngine0Mass:
	case ETech::ShipEngine0EPR:
	case ETech::ShipEngine0EMR:
		check(Ship);
		ValPtr = &Ship->LevelEngine0;
		break;
	case ETech::ShipEngine1Power:
	case ETech::ShipEngine1Mass:
	case ETech::ShipEngine1EPR:
	case ETech::ShipEngine1EMR:
		check(Ship);
		ValPtr = &Ship->LevelEngine1;
		break;
	case ETech::ShipEngine2Power:
	case ETech::ShipEngine2Mass:
	case ETech::ShipEngine2EPR:
	case ETech::ShipEngine2EMR:
		check(Ship);
		ValPtr = &Ship->LevelEngine2;
		break;
	case ETech::ShipEnergy0:
		check(Ship);
		ValPtr = &Ship->LevelEnergy0;
		break;
	case ETech::ShipEnergy1:
		check(Ship);
		ValPtr = &Ship->LevelEnergy1;
		break;
	case ETech::ShipEnergy2:
		check(Ship);
		ValPtr = &Ship->LevelEnergy2;
		break;
	}

	check(ValPtr);
	return ValPtr;
}

inline TArray<class UBackpackComponent*> UMenuTechHelper::GetBackpackList()
{
	TArray<UBackpackComponent*> BpList;
	AMainCharacter* Char = AMainLevelScript::GetMainChar();
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