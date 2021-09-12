// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBase.h"
#include "ItemDataObject.h"
#include "MainFunctionLibrary.h"
#include "MainCharacter.h"
#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include <UMG.h>

#define LOCTEXT_NAMESPACE "MenuBase"
#define DEBUG_INFO TEXT("没找到对应控件，请确认蓝图是否已修改")

void UMenuBase::NativePreConstruct()
{
	ListView_Makeable = Cast<UListView>(GetWidgetFromName(TEXT("ListView_Makeable")));
	ensureMsgf(ListView_Makeable, DEBUG_INFO);
	WB_NumberEditor = Cast<UUserWidget>(GetWidgetFromName(TEXT("WB_NumberEditor")));
	ensureMsgf(WB_NumberEditor, DEBUG_INFO);
	Button_Make = Cast<UButton>(GetWidgetFromName(TEXT("Button_Make")));
	ensureMsgf(Button_Make, DEBUG_INFO);
	Image_Item = Cast<UImage>(GetWidgetFromName(TEXT("Image_Item")));
	ensureMsgf(Image_Item, DEBUG_INFO);
	TextBlock_Name = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Name")));
	ensureMsgf(TextBlock_Name, DEBUG_INFO);
	TextBlock_Owned = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Owned")));
	ensureMsgf(TextBlock_Owned, DEBUG_INFO);
	TextBlock_Desc = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Desc")));
	ensureMsgf(TextBlock_Desc, DEBUG_INFO);
	TextBlock_Demand = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Demand")));
	ensureMsgf(TextBlock_Demand, DEBUG_INFO);

	ListView_Makeable->OnItemSelectionChanged().AddUObject(this, &UMenuBase::OnItemSelectionChanged);
	Cast<USpinBox>(WB_NumberEditor->GetWidgetFromName(TEXT("SpinBox_Number")))->OnValueChanged.AddDynamic(this, &UMenuBase::OnMakeCountChanged);
	Button_Make->OnClicked.AddDynamic(this, &UMenuBase::OnMakeClicked);

	return Super::NativePreConstruct();
}

void UMenuBase::NativeConstruct()
{
	TArray<FName> MakeableNames = UMainFunctionLibrary::GetMakeableItemList();
	for (const FName& RowName : MakeableNames)
	{
		UItemDataObject* Obj = NewObject<UItemDataObject>(this);
		Obj->RowName = RowName;
		ListView_Makeable->AddItem(Obj);
	}
	ListView_Makeable->SetSelectedIndex(0);

	return Super::NativeConstruct();
}

int32 UMenuBase::GetMaxMakeableCount(const FName& RowName)
{
	FMakeableItemData& MakeableData = UMainFunctionLibrary::GetMakeableItemData(RowName);
	UBackpackComponent* Storehouse = AMainCharacter::GetInstance()->GetBackpack(); //TODO 换回基地仓库
	int32 Value = MAX_int32;
	for (const auto& Demand : MakeableData.DemandList)
	{
		Value = FMath::Min(Value, Storehouse->CountItem(Demand.Key) / Demand.Value);
	}

	return Value;
}

void UMenuBase::OnItemSelectionChanged(UObject* Item)
{
	// 只能使用基地仓库的物品
	UItemDataObject* SelItem = Cast<UItemDataObject>(Item);
	FBasicItemData& ItemData = UMainFunctionLibrary::GetBasicItemData(SelItem->RowName);
	UBackpackComponent* Storehouse = AMainCharacter::GetInstance()->GetBackpack(); //TODO 换回基地仓库
	Image_Item->SetBrushFromSoftTexture(ItemData.Icon);
	TextBlock_Name->SetText(ItemData.Name);
	TextBlock_Owned->SetText(FText::Format(LOCTEXT("Owned", "拥有：x{0}"), Storehouse->FindItem(SelItem->RowName)));
	TextBlock_Desc->SetText(ItemData.Desc);
	
	USpinBox* SpinBox = Cast<USpinBox>(WB_NumberEditor->GetWidgetFromName(TEXT("SpinBox_Number")));
	int32 MakeableCount = GetMaxMakeableCount(SelItem->RowName);
	SpinBox->SetMinValue(MakeableCount > 0 ? 1 : 0);
	SpinBox->SetMaxValue(MakeableCount);
	// 这里会发出OnValueChanged广播，不需要再显式调用OnMakeCountChanged
	SpinBox->SetValue(SpinBox->GetMinValue());
}

void UMenuBase::OnMakeClicked()
{
	UItemDataObject* SelItem = Cast<UItemDataObject>(ListView_Makeable->GetSelectedItem());
	FMakeableItemData& MakeableData = UMainFunctionLibrary::GetMakeableItemData(SelItem->RowName);
	UBackpackComponent* Storehouse = AMainCharacter::GetInstance()->GetBackpack(); //TODO 换回基地仓库
	int32 Count = Cast<USpinBox>(WB_NumberEditor->GetWidgetFromName(TEXT("SpinBox_Number")))->GetValue();
	for (const auto& Demand : MakeableData.DemandList)
	{
		Storehouse->RemoveItem(Demand.Key, Demand.Value * Count);
	}
	Storehouse->AddItem(SelItem->RowName, Count);

	// 制作完之后需要更新一遍
	OnItemSelectionChanged(SelItem);
}

void UMenuBase::OnMakeCountChanged(float Count)
{
	UItemDataObject* SelItem = Cast<UItemDataObject>(ListView_Makeable->GetSelectedItem());
	FMakeableItemData& MakeableData = UMainFunctionLibrary::GetMakeableItemData(SelItem->RowName);
	FString DemandList;
	for (const auto& Demand : MakeableData.DemandList)
	{
		FBasicItemData& DemandData = UMainFunctionLibrary::GetBasicItemData(Demand.Key);
		DemandList += (DemandData.Name.ToString() + " x" + FString::FromInt(Demand.Value * FMath::Max((int32)Count, 1)));
		DemandList += "\n";
	}
	TextBlock_Demand->SetText(FText::FromString(DemandList));

	Button_Make->SetIsEnabled(Count > 0);
}

#undef LOCTEXT_NAMESPACE
