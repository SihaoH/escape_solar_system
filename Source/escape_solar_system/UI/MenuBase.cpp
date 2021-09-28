// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBase.h"
#include "ItemDataObject.h"
#include "MainFunctionLibrary.h"
#include "MainCharacter.h"
#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include <UMG.h>

#define LOCTEXT_NAMESPACE "MenuBase"

void UMenuBase::InitMakeableListView(class UListView* ListView)
{
	TArray<FName> MakeableNames = UMainFunctionLibrary::GetMakeableItemList();
	for (const FName& RowName : MakeableNames)
	{
		UItemDataObject* Obj = NewObject<UItemDataObject>(this);
		Obj->RowName = RowName;
		ListView->AddItem(Obj);
	}
	ListView->SetSelectedIndex(0);
}

int32 UMenuBase::GetMaxMakeableCount(const FName& RowName) const
{
	FMakeableItemData& MakeableData = UMainFunctionLibrary::GetMakeableItemData(RowName);
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

void UMenuBase::OnItemSelectionChanged(UObject* Item, float& MakeableMin, float& MakeableMax)
{
	// 只能使用基地仓库的物品
	UItemDataObject* SelItem = Cast<UItemDataObject>(Item);
	if (!SelItem) return;

	FBasicItemData& ItemData = UMainFunctionLibrary::GetBasicItemData(SelItem->RowName);
	UBackpackComponent* Storehouse = GetStorehouse();
	ItemIcon.ImageSize = FVector2D(64.f, 64.f);
	ItemIcon.SetResourceObject(ItemData.Icon.LoadSynchronous());
	ItemName = ItemData.Name;
	ItemDesc = ItemData.Desc;
	ItemMass = FText::Format(INVTEXT("{0} kg"), ItemData.Mass);
	ItemCount = Storehouse ? FText::AsNumber(Storehouse->CountItem(SelItem->RowName)) : LOCTEXT("unknow", "？？？");

	UnmakeableVisibility = Storehouse ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
	MakeableVisibility = Storehouse ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	
	int32 MakeableCount = GetMaxMakeableCount(SelItem->RowName);
	MakeableMin = MakeableCount > 0 ? 1 : 0;
	MakeableMax = MakeableCount;
}

void UMenuBase::MakeItem(UObject* SelItem, float Count)
{
	UItemDataObject* Item = Cast<UItemDataObject>(SelItem);
	FMakeableItemData& MakeableData = UMainFunctionLibrary::GetMakeableItemData(Item->RowName);
	UBackpackComponent* Storehouse = GetStorehouse();
	check(Storehouse);
	for (const TPair<FName, int32>& Demand : MakeableData.DemandList)
	{
		Storehouse->RemoveItem(Demand.Key, Demand.Value * Count);
	}
	Storehouse->AddItem(Item->RowName, Count);
}

void UMenuBase::OnMakeCountChanged(UObject* SelItem, float Count, FText& DemandText)
{
	UItemDataObject* Item = Cast<UItemDataObject>(SelItem);
	FMakeableItemData& MakeableData = UMainFunctionLibrary::GetMakeableItemData(Item->RowName);
	FString DemandList;
	for (const TPair<FName, int32>& Demand : MakeableData.DemandList)
	{
		
		FBasicItemData& DemandData = UMainFunctionLibrary::GetBasicItemData(Demand.Key);
		int32 NeedCount = Demand.Value * FMath::Max((int32)Count, 1);
		int32 HoldCount = GetStorehouse() ? GetStorehouse()->CountItem(Demand.Key) : 0;
		FStringFormatOrderedArguments Arguments;
		Arguments.Add(DemandData.Name.ToString());
		Arguments.Add(HoldCount > NeedCount ? TEXT("Default") : TEXT("Warning"));
		Arguments.Add(FString::FromInt(NeedCount));
		Arguments.Add(GetStorehouse() ? FString::FromInt(HoldCount) : LOCTEXT("unknow", "？？？").ToString());
		DemandList += FString::Format(TEXT("{0}  <{1}>x{2}</>  (<img id=\"Storehouse\"/> x{3})\n"), Arguments);
	}
	DemandText = FText::FromString(DemandList);
}

class UBackpackComponent* UMenuBase::GetStorehouse() const
{
	AEarthBaseActor* EarthBase = AMainCharacter::GetInstance()->FindEarthBase();
	return EarthBase ? EarthBase->GetBackpack() : nullptr;
}

#undef LOCTEXT_NAMESPACE
