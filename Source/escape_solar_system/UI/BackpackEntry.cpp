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

void UBackpackEntry::NativePreConstruct()
{
	Image_Icon = Cast<UImage>(GetWidgetFromName(TEXT("Image_Icon")));
	Image_Border = Cast<UImage>(GetWidgetFromName(TEXT("Image_Border")));
	TextBlock_Count = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Count")));
}

void UBackpackEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	SelfItem = ListItemObject;
	UItemDataObject* ItemObj = Cast<UItemDataObject>(ListItemObject);
	FBasicItemData& ItemDat = UMainFunctionLibrary::GetBasicItemData(ItemObj->RowName);
	Image_Icon->SetBrushFromSoftTexture(ItemDat.Icon);
	TextBlock_Count->SetText(ItemObj->Count > 0 ? FText::AsNumber(ItemObj->Count) : FText());
	SetSelectedStyle(SelectedItem == ItemObj);
	
	SetVisibility(ItemObj->RowName == EMPTY_NAME ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Visible);
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
	OutOperation->DefaultDragVisual = Image_Icon;
}

void UBackpackEntry::SetSelectedStyle(bool bIsSelected)
{
	if (bIsSelected)
	{
		Image_Border->SetColorAndOpacity(FLinearColor(1.f, 0.5f, 0.5f));
	}
	else
	{
		Image_Border->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f));
	}
}
