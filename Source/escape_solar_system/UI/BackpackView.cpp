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

bool UBackpackView::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	IUserObjectListEntry* Entry = Cast<IUserObjectListEntry>(InOperation->Payload);
	UListView* OtherItemView = Cast<UListView>(Entry->GetOwningListView());
	if (OtherItemView != Cast<UListView>(GetWidgetFromName(TEXT("ListView_Item"))))
	{
		UItemDataObject* ItemObj = Cast<UItemDataObject>(Entry->GetListItem());
		OnItemDrop.Broadcast(Backpack, ItemObj->Owner, ItemObj->RowName);
		return true;
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UBackpackView::InitEntryWidget(UUserWidget* Entry)
{
	// OnEntryClicked由MenuBackpack发出广播，这里绑定是需要通知到每个Entry
	// 由于C++没有对应的接口来获取生成的Entry，所以这里交给BP调用
	OnEntryClicked.AddUniqueDynamic(Cast<UBackpackEntry>(Entry), &UBackpackEntry::OnEntryClicked);
}

void UBackpackView::ClickItem(UObject* Item)
{
	OnItemClicked.Broadcast(Item);
}

void UBackpackView::OnBackpackChanged()
{
	float CurMass = Backpack->GetCurMass();
	FText MaxBearing = Backpack->MaxBearing < 0 ? INVTEXT("∞") : FText::AsNumber(Backpack->MaxBearing);
	InfoBearing = FText::Format(LOCTEXT("Bearing", "承重(kg): {0}/{1}"), CurMass, MaxBearing);

	TArray<UItemDataObject*> OutItems;
	const auto& ItemList = Backpack->GetItemList();
	for (const auto& Elem : ItemList)
	{
		UItemDataObject* Obj = NewObject<UItemDataObject>(this);
		Obj->RowName = Elem.Key;
		Obj->Count = Elem.Value;
		Obj->Owner = Backpack;
		OutItems.Add(Obj);
	}
	OnListViewResetted(OutItems);
	OnItemClicked.Broadcast(nullptr);
}

#undef LOCTEXT_NAMESPACE
