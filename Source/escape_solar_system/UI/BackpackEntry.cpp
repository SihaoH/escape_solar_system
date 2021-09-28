// Copyright 2020 H₂S. All Rights Reserved.

#include "BackpackEntry.h"
#include "ItemDataObject.h"
#include "MainFunctionLibrary.h"
#include "BackpackComponent.h"
#include <UMG.h>
#include <Blueprint/WidgetBlueprintLibrary.h>

UObject* UBackpackEntry::SelectedItem = nullptr;

void UBackpackEntry::OnEntryClicked(UObject* Item)
{
	SelectedItem = Item;
	SetSelectedStyle(SelfItem == Item);
}

void UBackpackEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	SelfItem = ListItemObject;
	UItemDataObject* ItemObj = Cast<UItemDataObject>(ListItemObject);
	FBasicItemData& ItemDate = UMainFunctionLibrary::GetBasicItemData(ItemObj->RowName);
	OnListItemObjectSetted(ItemDate.Icon, ItemDate.Name, ItemDate.CanStack ? FText::AsNumber(ItemObj->Count) : FText());
	SetSelectedStyle(SelectedItem == ItemObj);
}

//TODO 为毛这里不写也可以触发拖动？
//FReply UBackpackEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
//}

void UBackpackEntry::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	OutOperation = NewObject<UDragDropOperation>();
	OutOperation->Payload = this;
	OutOperation->DefaultDragVisual = this;
}

void UBackpackEntry::SetSelectedStyle(bool bIsSelected)
{
	OnSelectedStyleChanged(bIsSelected ? FLinearColor(1.f, 0.5f, 0.5f) : FLinearColor(1.f, 1.f, 1.f));
}
