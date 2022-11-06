// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBase.h"
#include "ItemDataObject.h"
#include "MainLibrary.h"
#include "MainLevelScript.h"
#include "MainCharacter.h"
#include "EarthBaseActor.h"
#include "CelestialBody.h"
#include "MainPlayerState.h"
#include "BackpackComponent.h"
#include "Spaceship.h"
#include "MainLevelScript.h"
#include <UMG.h>


FString UMenuBaseHelper::GetGameDate() const
{
	static const FDateTime InitialDate(2077, 07, 31);
	constexpr float MapVal = 86400.f / 300.f;
	float RealTime = AMainPlayerState::Instance()->GetTotalTime() + AMainLevelScript::Instance()->GetWorld()->GetTimeSeconds();
	return (InitialDate + FTimespan::FromSeconds(MapVal * RealTime)).ToString(TEXT("%Y/%m/%d %H:00"));
}

void UMenuBaseHelper::GetPlanetDistance(TMap<FString, float>& List) const
{
	auto BaseLoc = AMainLevelScript::GetEarthBase()->GetActorLocation();
	for (TActorIterator<ACelestialBody> It(AMainLevelScript::Instance()->GetWorld()); It; ++It)
	{
		ACelestialBody* CB = *It;
		if (CB->GetCelestialType() == ECelestialBodyType::Planet)
		{
			List.Add(CB->GetLabelName().ToString(), FVector::Dist(BaseLoc, CB->GetActorLocation()));
		}
	}
}

float UMenuBaseHelper::GetShipDistance() const
{
	auto SpaceShip = AMainLevelScript::GetSpaceship();
	if (SpaceShip)
	{
		return SpaceShip->GetDistanceTo(AMainLevelScript::GetEarthBase()) / 100;
	}
	return -1;
}

void UMenuBaseHelper::GetMakeableList(TArray<class UItemDataObject*>& OutItems) const
{
	TArray<FName> MakeableNames = UMainLibrary::GetMakeableItemList();
	for (const FName& RowName : MakeableNames)
	{
		UItemDataObject* Obj = NewObject<UItemDataObject>(const_cast<UMenuBaseHelper*>(this));
		Obj->RowName = RowName;
		OutItems.Add(Obj);
	}
}

int32 UMenuBaseHelper::GetMaxMakeableCount(const FName& RowName) const
{
	FItemData& MakeableData = UMainLibrary::GetItemData(RowName);
	int32 RowNumber = FCString::Atoi(*(RowName.ToString()));
	int32 Value = 0;
	UBackpackComponent* Storehouse = GetStorehouse();
	if (Storehouse)
	{
		Value = MAX_int32;
		for (const TPair<FName, int32>& Demand : MakeableData.DemandList)
		{
			Value = FMath::Min(Value, Storehouse->CountItem(Demand.Key) / Demand.Value);
		}
	}

	// 飞船的物品编号，只能制作一台
	if (RowNumber == 9999)
	{
		if (AMainLevelScript::GetSpaceship())
		{
			Value = 0;
		}
		else if (Value > 0)
		{
			Value = 1;
		}
	}
	return Value;
}

int32 UMenuBaseHelper::GetHoldCount(const FName& RowName) const
{
	return GetStorehouse()->CountItem(RowName);
}

void UMenuBaseHelper::MakeItem(UObject* SelItem, float Count)
{
	UItemDataObject* Item = Cast<UItemDataObject>(SelItem);
	FItemData& MakeableData = UMainLibrary::GetItemData(Item->RowName);
	UBackpackComponent* Storehouse = GetStorehouse();
	check(Storehouse);
	for (const TPair<FName, int32>& Demand : MakeableData.DemandList)
	{
		Storehouse->RemoveItem(Demand.Key, Demand.Value * Count);
	}
	int32 RowNumber = FCString::Atoi(*(Item->RowName.ToString()));
	if (RowNumber == 9999)
	{
		AMainLevelScript::GetEarthBase()->CreateSpaceship();
	}
	else
	{
		Storehouse->AddItem(Item->RowName, Count);
	}
}

class UBackpackComponent* UMenuBaseHelper::GetStorehouse() const
{
	AEarthBase* EarthBase = AMainLevelScript::GetMainChar()->FindEarthBase();
	return EarthBase ? EarthBase->Backpack : nullptr;
}
