// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBase.h"
#include "ItemDataObject.h"
#include "MainFunctionLibrary.h"
#include "MainCharacter.h"
#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include <UMG.h>

#define LOCTEXT_NAMESPACE "MenuBase"

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
	UBackpackComponent* Storehouse = GetStorehouse();
	if (Storehouse) {
		int32 Value = MAX_int32;
		for (const TPair<FName, int32>& Demand : MakeableData.DemandList)
		{
			Value = FMath::Min(Value, Storehouse->CountItem(Demand.Key) / Demand.Value);
		}
		return Value;
	}
	return 0;
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
	Storehouse->AddItem(Item->RowName, Count);
}

class UBackpackComponent* UMenuBaseHelper::GetStorehouse() const
{
	AEarthBase* EarthBase = AMainLevelScriptActor::GetMainChar()->FindEarthBase();
	return EarthBase ? EarthBase->Backpack : nullptr;
}

#undef LOCTEXT_NAMESPACE
