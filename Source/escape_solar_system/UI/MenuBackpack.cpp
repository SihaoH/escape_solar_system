// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBackpack.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "BackpackView.h"
#include "ItemDataObject.h"
#include "BackpackComponent.h"
#include "BodyComponent.h"
#include "EngineComponent.h"


void UMenuBackpackHelper::SelectItem(UObject* Item)
{
	SelectedItem = Cast<UItemDataObject>(Item);
	check(SelectedItem);
}

void UMenuBackpackHelper::UseItem()
{
	SelectedItem->Owner->RemoveItem(SelectedItem->RowName, SelectedItem->Count);
	SelectedItem = nullptr;
}

void UMenuBackpackHelper::DropItem(int32 Count)
{
	if (DstBackpack && SrcBackpack && SelectedItem)
	{
		DstBackpack->AddItem(SelectedItem->RowName, Count);
		SrcBackpack->RemoveItem(SelectedItem->RowName, Count);
	}
	DstBackpack = nullptr;
	SrcBackpack = nullptr;
	SelectedItem = nullptr;
}

void UMenuBackpackHelper::ConsumeItem(int32 Count, EPawnType Target)
{
	AMainCharacter* Char = AMainLevelScriptActor::GetMainChar();
	ASpaceship* Ship = Char->FindSpaceship();
	UBodyComponent* Body = nullptr;
	UEngineComponent* Engine = nullptr;
	// TODO Target参数要换成枚举
	if (Target == EPawnType::MainChar)
	{
		Body = Char->Body;
		Engine = Char->Engine;
	}
	else if (Target == EPawnType::SpaceShip && Ship)
	{
		Body = Ship->Body;
		Engine = Ship->Engine;
	}

	check(Body);
	check(Engine);
	check(SelectedItem);
	int32 RowName = FCString::Atoi(*(SelectedItem->RowName.ToString()));
	float Value = UMainFunctionLibrary::GetItemData(SelectedItem->RowName).ReplenishedValue;
	if (RowName >= 3000 && RowName < 4000)
	{
		Body->ChangeHP(Value);
	}
	else if (RowName >= 4000 && RowName < 5000)
	{
		Engine->ChangeEnergy(Value);
	}
	SelectedItem->Owner->RemoveItem(SelectedItem->RowName, Count);

	SelectedItem = nullptr;
}

void UMenuBackpackHelper::DiscardItem(int32 Count)
{
	if (SelectedItem)
	{
		SelectedItem->Owner->RemoveItem(SelectedItem->RowName, Count);
	}
	SelectedItem = nullptr;
}

void UMenuBackpackHelper::TryDropItem(UBackpackComponent* DstBp, UBackpackComponent* SrcBp, UObject* Item)
{
	SelectItem(Item);
	int32 MaxAdd = FMath::Min(SrcBp->CountItem(SelectedItem->RowName), DstBp->GetMaxAddNum(SelectedItem->RowName));
	if (MaxAdd > 0)
	{
		DstBackpack = DstBp;
		SrcBackpack = SrcBp;

		FItemData& ItemData = UMainFunctionLibrary::GetItemData(SelectedItem->RowName);
		if (!ItemData.CanStack || SelectedItem->Count <= 1)
		{
			//DropItem(1);
			// TODO 单个物品直接放入，界面不要弹数量对话框
		}
		else
		{
			
		}
		
		//UUserWidget* WB_Dlg_ItemNum = Cast<UUserWidget>(GetWidgetFromName(TEXT("WB_Dialog_ItemNumber")));
		//USpinBox* SpinBox = Cast<USpinBox>(Cast<UUserWidget>(WB_Dlg_ItemNum->GetWidgetFromName(TEXT("WB_NumberEditor")))->GetWidgetFromName(TEXT("SpinBox_Number")));
		//SpinBox->SetMinValue(1);
		//SpinBox->SetMaxValue(MaxAdd);
		//SpinBox->SetValue(MaxAdd);
		//WB_Dlg_ItemNum->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// TODO 不能放入，需要其他方式的提示
	}
}

int32 UMenuBackpackHelper::GetSelectedCount() const
{
	return SelectedItem ? SelectedItem->Count : 0;
}

void UMenuBackpackHelper::GetMenuStatus(bool& CanUse, bool& CanConsume)
{
	CanUse = false;
	CanConsume = false;
	check(SelectedItem);

	int32 RowName = FCString::Atoi(*(SelectedItem->RowName.ToString()));
	if (RowName > 9000)
	{
		CanUse = true;
	}
	else if (RowName >= 3000 && RowName < 5000)
	{
		CanConsume = true;
	}
}
