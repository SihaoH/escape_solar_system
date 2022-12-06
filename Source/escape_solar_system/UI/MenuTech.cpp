// Copyright 2020 H₂S. All Rights Reserved.


#include "MenuTech.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include "MainLibrary.h"
#include "MainPlayerState.h"
#include "MainLevelScript.h"

const int Max_Level = 4;

UMenuTechHelper::UMenuTechHelper()
{
	ConstructorHelpers::FObjectFinder<UDataTable> Finder_Info(TEXT("DataTable'/Game/DataTable/DT_TechInfo.DT_TechInfo'"));
	DT_TechInfo = Finder_Info.Object;
	check(DT_TechInfo);
}

void UMenuTechHelper::SelectTech(const TArray<ETech>& Props)
{
	DemandPoints = 0;
	DemandItems.Empty(0);

	ETech Tech = Props[0];
	int Val = *GetTarget(Tech);
	if (Val < Max_Level)
	{
		auto PlayerState = AMainPlayerState::Instance();

		FTechDemand LevelDemand = GetTechDemand(Tech, Val+1);
		// 已升级过的技能不消耗探索点
		DemandPoints = Val >= PlayerState->GetBestLevel(Props) ? LevelDemand.Points : 0;

		AEarthBase* EarthBase = AMainLevelScript::GetMainChar()->FindEarthBase();
		UBackpackComponent* Backpack = EarthBase ? EarthBase->Backpack : nullptr;
		bool Enough = GetDemandInfo(LevelDemand.Items, Backpack);
		CanUpgrade = Enough && (PlayerState->GetExplorePoints() >= LevelDemand.Points);
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
		FTechDemand LevelDemand = GetTechDemand(Level, *ValPtr+1);
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
	FTechInfo* Info = DT_TechInfo ? DT_TechInfo->FindRow<FTechInfo>(TECH::InfoRow[Level], FString()) : nullptr;
	if (Info)
	{
		return Info->Name;
	}
	return FText();
}

FText UMenuTechHelper::GetTechDesc(const TArray<ETech>& Props)
{
	ETech Level = Props[0];
	FTechInfo* Info = DT_TechInfo ? DT_TechInfo->FindRow<FTechInfo>(TECH::InfoRow[Level], FString()) : nullptr;
	if (Info)
	{
		return Info->Desc;
	}
	return FText();
}

TSoftObjectPtr<UTexture2D> UMenuTechHelper::GetTechIcon(const TArray<ETech>& Props)
{
	ETech Level = Props[0];
	FTechInfo* Info = DT_TechInfo ? DT_TechInfo->FindRow<FTechInfo>(TECH::InfoRow[Level], FString()) : nullptr;
	if (Info)
	{
		return Info->Icon;
	}
	return nullptr;
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

FText UMenuTechHelper::GetTechValueName(ETech Prop)
{
	return UMainLibrary::GetTechValueName(Prop);
}

float UMenuTechHelper::GetTechValueVal(ETech Prop, bool bNext)
{
	const int Level = *GetTarget(Prop);
	return UMainLibrary::GetTechValueVal(Prop, bNext ? Level+1 : Level);
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

int* UMenuTechHelper::GetTarget(ETech Tech)
{
	auto Char = AMainLevelScript::GetMainChar();
	auto Ship = AMainLevelScript::GetSpaceship();

	int* ValPtr = nullptr;
	switch (Tech)
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

FTechDemand UMenuTechHelper::GetTechDemand(ETech Tech, int32 Val)
{
	FTechInfo* Info = DT_TechInfo ? DT_TechInfo->FindRow<FTechInfo>(TECH::InfoRow[Tech], FString()) : nullptr;
	if (Info)
	{
		if (Val >= 0 && Val < Info->DemandList.Num())
		{
			return Info->DemandList[Val];
		}
	}
	return FTechDemand();
}

bool UMenuTechHelper::GetDemandInfo(const TMap<FName, int32>& List, UBackpackComponent* Backpack)
{
	bool Enough = true;
	for (const TPair<FName, int32>& Demand : List)
	{
		const FItemData& DemandData = UMainLibrary::GetItemData(Demand.Key);
		const int32 NeedCount = Demand.Value;
		const int32 HoldCount = Backpack ? Backpack->CountItem(Demand.Key) : -1;
		DemandItems.Add(FTechUpgradeDemand(DemandData.Name, HoldCount, NeedCount));
		if (HoldCount < NeedCount)
		{
			Enough = false;
		}
	}
	return Enough;
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
