// Copyright 2020 H₂S. All Rights Reserved.

#include "BackpackView.h"
#include "BackpackComponent.h"
#include "ItemDataObject.h"
#include <UMG.h>

#define LOCTEXT_NAMESPACE "BackpackView"

void UBackpackView::SetBackpack(UBackpackComponent* Bp)
{
	BpComp = Bp;
	if (BpComp)
	{
		SetVisibility(ESlateVisibility::Visible);
		OnBackpackChanged();
		BpComp->OnChanged.AddUniqueDynamic(this, &UBackpackView::OnBackpackChanged);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBackpackView::ClearSelection()
{
	TileView_Item->ClearSelection();
}

void UBackpackView::NativePreConstruct()
{
	TileView_Item = Cast<UTileView>(GetWidgetFromName(TEXT("TileView_Item")));
	TextBlock_Bearing = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Bearing")));
	TileView_Item->OnItemClicked().AddUObject(this, &UBackpackView::OnTileViewItemClicked);

	return Super::NativePreConstruct();
}

bool UBackpackView::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	IUserObjectListEntry* Entry = Cast<IUserObjectListEntry>(InOperation->Payload);
	UTileView* OtherItemView = Cast<UTileView>(Entry->GetOwningListView());
	if (OtherItemView != TileView_Item)
	{
		// TODO 需要弹出选择个数对话框，按用户输入的数量进行操作
		UItemDataObject* ItemObj = Cast<UItemDataObject>(Entry->GetListItem());
		int32 OtherIndex = OtherItemView->GetIndexForItem(ItemObj);
		int32 AddedCount = BpComp->AddItem(ItemObj->RowName, ItemObj->Count);
		ItemObj->Owner->RemoveItem(OtherIndex, AddedCount);
		return true;
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UBackpackView::OnBackpackChanged()
{
	float CurMass = BpComp->GetCurMass();
	FText MaxBearing = BpComp->MaxBearing < 0 ? INVTEXT("∞") : FText::AsNumber(BpComp->MaxBearing);
	TextBlock_Bearing->SetText(FText::Format(LOCTEXT("Bearing", "承重(kg): {0}/{1}"), CurMass, MaxBearing));

	TileView_Item->ClearListItems();
	const TArray<FBackpackItemInfo>& ItemList = BpComp->GetItemList();
	for (int32 i = 0; i < BpComp->MaxGrid; i++)
	{
		UItemDataObject* Obj = NewObject<UItemDataObject>(this);
		Obj->RowName = ItemList[i].RowName;
		Obj->Count = ItemList[i].Count;
		Obj->Owner = BpComp;
		TileView_Item->AddItem(Obj);
	}
	OnItemClicked.Broadcast(this, nullptr);
}

void UBackpackView::OnTileViewItemClicked(UObject* Item)
{
	OnItemClicked.Broadcast(this, Item);
}

#undef LOCTEXT_NAMESPACE
