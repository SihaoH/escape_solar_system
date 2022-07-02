// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBase.h"
#include "ItemDataObject.h"
#include "MainFunctionLibrary.h"
#include "MainLevelScriptActor.h"
#include "MainCharacter.h"
#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include "Spaceship.h"
#include "MainLevelScriptActor.h"
#include <UMG.h>

#define LOCTEXT_NAMESPACE "MenuBase"

float UMenuBaseHelper::GetShipDistance() const
{
	auto SpaceShip = AMainLevelScriptActor::GetSpaceship();
	if (SpaceShip)
	{
		return SpaceShip->GetDistanceTo(AMainLevelScriptActor::GetEarthBase()) / 100;
	}
	return -1;
}

void UMenuBaseHelper::GetMakeableList(TArray<class UItemDataObject*>& OutItems) const
{
	TArray<FName> MakeableNames = UMainFunctionLibrary::GetMakeableItemList();
	for (const FName& RowName : MakeableNames)
	{
		UItemDataObject* Obj = NewObject<UItemDataObject>(const_cast<UMenuBaseHelper*>(this));
		Obj->RowName = RowName;
		OutItems.Add(Obj);
	}
}

int32 UMenuBaseHelper::GetMaxMakeableCount(const FName& RowName) const
{
	FItemData& MakeableData = UMainFunctionLibrary::GetItemData(RowName);
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
		if (AMainLevelScriptActor::GetSpaceship())
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
	FItemData& MakeableData = UMainFunctionLibrary::GetItemData(Item->RowName);
	UBackpackComponent* Storehouse = GetStorehouse();
	check(Storehouse);
	for (const TPair<FName, int32>& Demand : MakeableData.DemandList)
	{
		Storehouse->RemoveItem(Demand.Key, Demand.Value * Count);
	}
	int32 RowNumber = FCString::Atoi(*(Item->RowName.ToString()));
	if (RowNumber == 9999)
	{
		AMainLevelScriptActor::GetEarthBase()->CreateSpaceship();
	}
	else
	{
		Storehouse->AddItem(Item->RowName, Count);
	}
}

class UBackpackComponent* UMenuBaseHelper::GetStorehouse() const
{
	AEarthBase* EarthBase = AMainLevelScriptActor::GetMainChar()->FindEarthBase();
	return EarthBase ? EarthBase->Backpack : nullptr;
}

#undef LOCTEXT_NAMESPACE
