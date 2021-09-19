// Copyright 2020 H₂S. All Rights Reserved.

#include "BackpackView.h"
#include "BackpackEntry.h"
#include "BackpackComponent.h"
#include "ItemDataObject.h"
#include <UMG.h>

#define LOCTEXT_NAMESPACE "BackpackView"

void UBackpackView::SetBackpack(UBackpackComponent* Bp)
{
	Backpack = Bp;
	if (Backpack)
	{
		SetVisibility(ESlateVisibility::Visible);
		OnBackpackChanged();
		Backpack->OnChanged.AddUniqueDynamic(this, &UBackpackView::OnBackpackChanged);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBackpackView::OnEntryWidgetInited(UUserWidget* Entry)
{
	// OnEntryClicked由MenuBackpack发出广播，这里绑定是需要通知到每个Entry
	// 由于C++没有对应的接口，所以这里交给BP调用
	OnEntryClicked.AddUniqueDynamic(Cast<UBackpackEntry>(Entry), &UBackpackEntry::OnEntryClicked);
}

void UBackpackView::NativePreConstruct()
{
	TileView_Item = Cast<UTileView>(GetWidgetFromName(TEXT("TileView_Item")));
	TileView_Item->OnItemClicked().AddUObject(this, &UBackpackView::OnTileViewItemClicked);

	return Super::NativePreConstruct();
}

bool UBackpackView::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	IUserObjectListEntry* Entry = Cast<IUserObjectListEntry>(InOperation->Payload);
	UTileView* OtherItemView = Cast<UTileView>(Entry->GetOwningListView());
	if (OtherItemView != TileView_Item)
	{
		UItemDataObject* ItemObj = Cast<UItemDataObject>(Entry->GetListItem());
		OnItemDrop.Broadcast(Backpack, ItemObj->Owner, OtherItemView->GetIndexForItem(ItemObj));
		return true;
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UBackpackView::OnBackpackChanged()
{
	float CurMass = Backpack->GetCurMass();
	FText MaxBearing = Backpack->MaxBearing < 0 ? INVTEXT("∞") : FText::AsNumber(Backpack->MaxBearing);
	InfoBearing = FText::Format(LOCTEXT("Bearing", "承重(kg): {0}/{1}"), CurMass, MaxBearing);

	TileView_Item->ClearListItems();
	const TArray<FBackpackItemInfo>& ItemList = Backpack->GetItemList();
	for (int32 i = 0; i < Backpack->MaxGrid; i++)
	{
		UItemDataObject* Obj = NewObject<UItemDataObject>(this);
		Obj->Index = i;
		Obj->RowName = ItemList[i].RowName;
		Obj->Count = ItemList[i].Count;
		Obj->Owner = Backpack;
		TileView_Item->AddItem(Obj);
	}
	OnItemClicked.Broadcast(nullptr);
}

void UBackpackView::OnTileViewItemClicked(UObject* Item)
{
	OnItemClicked.Broadcast(Item);
}

#undef LOCTEXT_NAMESPACE
