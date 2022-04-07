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

void UMenuBaseHelper::OnItemSelectionChanged(UObject* SelItem, float& MakeableMin, float& MakeableMax)
{
	// 只能使用基地仓库的物品
	UItemDataObject* Item = Cast<UItemDataObject>(SelItem);
	if (!Item) return;

	FItemData& ItemData = UMainFunctionLibrary::GetItemData(Item->RowName);
	UBackpackComponent* Storehouse = GetStorehouse();
	ItemIcon.ImageSize = FVector2D(64.f, 64.f);
	ItemIcon.SetResourceObject(ItemData.Icon.LoadSynchronous());
	ItemName = ItemData.Name;
	ItemDesc = ItemData.Desc;
	ItemMass = FText::Format(INVTEXT("{0} kg"), ItemData.Mass);
	ItemCount = Storehouse ? FText::AsNumber(Storehouse->CountItem(Item->RowName)) : LOCTEXT("unknow", "？？？");

	UnmakeableVisibility = Storehouse ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
	MakeableVisibility = Storehouse ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	
	int32 MakeableCount = GetMaxMakeableCount(Item->RowName);
	MakeableMin = MakeableCount > 0 ? 1 : 0;
	MakeableMax = MakeableCount;
}

void UMenuBaseHelper::OnMakeCountChanged(UObject* SelItem, float Count, FText& DemandText)
{
	UItemDataObject* Item = Cast<UItemDataObject>(SelItem);
	FItemData& MakeableData = UMainFunctionLibrary::GetItemData(Item->RowName);
	auto DemandInfo = UMainFunctionLibrary::GetDemandInfo(MakeableData.DemandList, GetStorehouse(), Count);
	CanMake = DemandInfo.Key;
	DemandText = DemandInfo.Value;
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

class UBackpackComponent* UMenuBaseHelper::GetStorehouse() const
{
	AEarthBase* EarthBase = AMainLevelScriptActor::GetMainChar()->FindEarthBase();
	return EarthBase ? EarthBase->Backpack : nullptr;
}

#undef LOCTEXT_NAMESPACE
