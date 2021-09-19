// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBackpack.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "BackpackView.h"
#include "ItemDataObject.h"
#include "BackpackComponent.h"
#include "MainFunctionLibrary.h"
#include <UMG.h>

#define LOCTEXT_NAMESPACE "MenuBackpack"

void UMenuBackpack::InitBpView(UBackpackView* Body, UBackpackView* Base, UBackpackView* Ship)
{
	BpView_Body = Body;
	BpView_Base = Base;
	BpView_Ship = Ship;
	BpView_Body->OnItemClicked.AddUniqueDynamic(this, &UMenuBackpack::OnBpViewItemClicked);
	BpView_Base->OnItemClicked.AddUniqueDynamic(this, &UMenuBackpack::OnBpViewItemClicked);
	BpView_Ship->OnItemClicked.AddUniqueDynamic(this, &UMenuBackpack::OnBpViewItemClicked);
	BpView_Body->OnItemDrop.AddUniqueDynamic(this, &UMenuBackpack::OnBpViewItemDrop);
	BpView_Base->OnItemDrop.AddUniqueDynamic(this, &UMenuBackpack::OnBpViewItemDrop);
	BpView_Ship->OnItemDrop.AddUniqueDynamic(this, &UMenuBackpack::OnBpViewItemDrop);

	CheckBpView();
}

void UMenuBackpack::AcceptDrop(float Count)
{
	if (DstBackpack && SrcBackpack && SrcIndex > -1)
	{
		FBackpackItemInfo ItemInfo = SrcBackpack->FindItem(SrcIndex);
		int32 AddNum = DstBackpack->AddItem(ItemInfo.RowName, Count);
		SrcBackpack->RemoveItem(SrcIndex, AddNum);
	}
	DstBackpack = nullptr;
	SrcBackpack = nullptr;
	SrcIndex = -1;
}

void UMenuBackpack::DiscardItem(float Count)
{
	if (SelectedItem)
	{
		SelectedItem->Owner->RemoveItem(SelectedItem->Index, Count);
	}
}

void UMenuBackpack::CheckBpView()
{
	AMainCharacter* MainCharacter = AMainCharacter::GetInstance();
	check(MainCharacter);
	AEarthBaseActor* EarthBase = MainCharacter->FindEarthBase();
	ASpaceship* Spaceship = MainCharacter->FindSpaceship();
	if (Spaceship == nullptr && EarthBase)
	{
		Spaceship = EarthBase->FindSpaceship();
	}

	BpView_Body->SetBackpack(MainCharacter->GetBackpack());
	BpView_Base->SetBackpack(EarthBase ? EarthBase->GetBackpack() : nullptr);
	BpView_Ship->SetBackpack(Spaceship ? Spaceship->GetBackpack() : nullptr);
}

void UMenuBackpack::SelectItem(UItemDataObject* Item)
{
	SelectedItem = Item;
	NoneVisibility = Item ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	ItemVisibility = Item ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	if (Item)
	{
		FBasicItemData& Data = UMainFunctionLibrary::GetBasicItemData(Item->RowName);
		ItemIcon.ImageSize = FVector2D(64.f, 64.f);
		ItemIcon.SetResourceObject(Data.Icon.LoadSynchronous());
		ItemName = Data.Name;
		ItemDesc = Data.Desc;
		ItemMass = FText::Format(LOCTEXT("Mass", "重量: {0}KG"), Data.Mass);
		ItemStack = FText::Format(LOCTEXT("Stack", "最大堆叠数: {0}"), Data.MaxStack);
		ItemCount = FText::Format(LOCTEXT("Count", "当前物品槽: {0}"), Item->Count);

		OnItemSelected(Item->Count);
	}
}

void UMenuBackpack::OnBpViewItemClicked(UObject* Item)
{
	SelectItem(Cast<UItemDataObject>(Item));

	BpView_Body->OnEntryClicked.Broadcast(Item);
	BpView_Base->OnEntryClicked.Broadcast(Item);
	BpView_Ship->OnEntryClicked.Broadcast(Item);
}

void UMenuBackpack::OnBpViewItemDrop(UBackpackComponent* DstBp, UBackpackComponent* SrcBp, int32 SrcIdx)
{
	FBackpackItemInfo ItemInfo = SrcBp->FindItem(SrcIdx);
	int32 MaxAdd = FMath::Min(ItemInfo.Count, DstBp->GetMaxAddNum(ItemInfo.RowName));
	if (MaxAdd > 0)
	{
		DstBackpack = DstBp;
		SrcBackpack = SrcBp;
		SrcIndex = SrcIdx;

		UUserWidget* WB_Dlg_ItemNum = Cast<UUserWidget>(GetWidgetFromName(TEXT("WB_Dialog_ItemNumber")));
		USpinBox* SpinBox = Cast<USpinBox>(Cast<UUserWidget>(WB_Dlg_ItemNum->GetWidgetFromName(TEXT("WB_NumberEditor")))->GetWidgetFromName(TEXT("SpinBox_Number")));
		SpinBox->SetMinValue(1);
		SpinBox->SetMaxValue(MaxAdd);
		SpinBox->SetValue(MaxAdd);
		WB_Dlg_ItemNum->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// TODO 不能放入，需要其他方式的提示
	}
}

#undef LOCTEXT_NAMESPACE
