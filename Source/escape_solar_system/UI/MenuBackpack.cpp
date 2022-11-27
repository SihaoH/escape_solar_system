// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBackpack.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "ItemDataObject.h"
#include "BackpackComponent.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include "MainLibrary.h"
#include "MainLevelScript.h"

static const TArray<FName> HPItems = { "小型维修包", "中型维修包", "大型维修包" };
static const TArray<FName> MPItems = { "化学燃料罐", "裂变燃料罐", "聚变燃料罐" };
static const TArray<FName> SPItems = { "特殊物品" };

void UMenuBackpackHelper::GetListViewItems(UBackpackComponent* InBp, TArray<class UItemDataObject*>& OutItems) const
{
	if (InBp) {
		const auto& ItemList = InBp->GetItemList();
		for (const auto& Elem : ItemList)
		{
			UItemDataObject* Obj = NewObject<UItemDataObject>(const_cast<UMenuBackpackHelper*>(this));
			Obj->RowName = Elem.Key;
			Obj->Count = Elem.Value;
			Obj->Owner = InBp;
			OutItems.Add(Obj);
		}
	}
}

/* 后续所有关于物品的操作，都依赖于这个已选中的成员变量
 * 即：操作前必须选中物品
*/
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
	AMainCharacter* Char = AMainLevelScript::GetMainChar();
	ASpaceship* Ship = Char->FindSpaceship();
	UBodyComponent* Body = nullptr;
	UEngineComponent* Engine = nullptr;

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
	FName RowName = SelectedItem->RowName;
	float Value = UMainLibrary::GetItemData(SelectedItem->RowName).ReplenishedValue * Count;
	if (HPItems.Contains(RowName))
	{
		Body->ChangeHP(Value);
	}
	else if (MPItems.Contains(RowName))
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

void UMenuBackpackHelper::TryDropItem(UBackpackComponent* DstBp, UBackpackComponent* SrcBp, bool& NeedHint, int32& MaxAdd, FText& Reason)
{
	check(SelectedItem);
	MaxAdd = FMath::Min(SrcBp->CountItem(SelectedItem->RowName), DstBp->GetMaxAddNum(SelectedItem->RowName));
	if (MaxAdd > 0)
	{
		DstBackpack = DstBp;
		SrcBackpack = SrcBp;

		FItemData& ItemData = UMainLibrary::GetItemData(SelectedItem->RowName);
		if (!ItemData.CanStack || SelectedItem->Count <= 1)
		{
			// 单个物品直接放入
			DropItem(1);
			NeedHint = false;
		}
		else
		{
			NeedHint = true;
		}
	}
	else
	{
		Reason = tr("无法放入");
		NeedHint = false;
	}
}

void UMenuBackpackHelper::GetMenuDisplay(TMap<EItemOptions, FText>& OutList)
{
	OutList.Add(EItemOptions::Use, tr("使用"));
	OutList.Add(EItemOptions::ConsumeToChar, tr("为躯体补充"));
	OutList.Add(EItemOptions::ConsumeToShip, tr("为飞船补充"));
	OutList.Add(EItemOptions::Discard, tr("丢弃"));
}

void UMenuBackpackHelper::GetMenuOptions(TArray<EItemOptions>& OutList)
{
	FName RowName = SelectedItem->RowName;
	if (SPItems.Contains(RowName))
	{
		OutList.Add(EItemOptions::Use);
	}
	else if (HPItems.Contains(RowName) || MPItems.Contains(RowName))
	{
		OutList.Add(EItemOptions::ConsumeToChar);
		if (AMainLevelScript::GetSpaceship())
		{
			OutList.Add(EItemOptions::ConsumeToShip);
		}
	}
	OutList.Add(EItemOptions::Discard);
}
