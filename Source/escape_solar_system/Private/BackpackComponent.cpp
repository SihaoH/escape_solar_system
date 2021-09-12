// Copyright 2020 H₂S. All Rights Reserved.

#include "BackpackComponent.h"
#include "MainFunctionLibrary.h"

UBackpackComponent::UBackpackComponent()
{
	bWantsInitializeComponent = true;
}

void UBackpackComponent::InitializeComponent()
{
	Super::InitializeComponent();
	ItemList.SetNum(MaxGrid);
}

int32 UBackpackComponent::GetMaxAddNum(const FName& RowName)
{
	check(MaxGrid > 0);
	if (GetCurGrid() >= MaxGrid)
	{
		return 0;
	}
	if (MaxBearing < 0)
	{
		return MAX_int32;
	}
	const FBasicItemData& ItemData = UMainFunctionLibrary::GetBasicItemData(RowName);
	float Remaining = MaxBearing - GetCurMass();
	return FMath::FloorToInt(Remaining / ItemData.Mass);
}

int32 UBackpackComponent::AddItem(const FName& RowName, int32 Count)
{
	const FBasicItemData& ItemData = UMainFunctionLibrary::GetBasicItemData(RowName);
	check(&ItemData);
	Count = FMath::Min(GetMaxAddNum(RowName), Count); //最多可放入的数量
	int32 Pending = Count; //放入过程中剩余未放入的量

	for (FBackpackItemInfo& ItemInfo : ItemList)
	{
		if (ItemInfo.RowName == RowName || ItemInfo.RowName == EMPTY_NAME)
		{
			ItemInfo.RowName = RowName;
			if (ItemInfo.Count + Pending > ItemData.MaxStack)
			{	// 放入的量超过了当前格子（物品）的最大堆叠量，先放一点
				Pending -= (ItemData.MaxStack - ItemInfo.Count);
				ItemInfo.Count = ItemData.MaxStack;
			}
			else
			{	//全部放完，退出循环
				ItemInfo.Count += Pending;
				Pending -= Pending;
				break;
			}
		}
	}

	if (Count - Pending > 0)
	{
		OnChanged.Broadcast();
	}
	return (Count - Pending);
}

void UBackpackComponent::RemoveItem(const FName& RowName, int32 Count)
{
	int32 Pending = Count; //丢弃过程中剩余未放入的量
	for (int i = 0; i < ItemList.Num(); i ++)
	{
		if (ItemList[i].RowName == RowName)
		{
			if (Pending >= ItemList[i].Count)
			{	//丢弃的量≥当前格子物品的量，先把该格子清空，再找下一个格子
				ItemList[i].Count -= ItemList[i].Count;
				Pending -= ItemList[i].Count;
				ItemList[i] = FBackpackItemInfo();
			}
			else
			{	//丢弃的量小于当前格子物品的量，直接丢
				ItemList[i].Count -= Pending;
				Pending -= Pending;
			}
			
			if (Pending <= 0)
			{
				break;
			}
		}
	}
	check(Pending > 0);

	OnChanged.Broadcast();
}

void UBackpackComponent::RemoveItem(int32 Index, int32 Count)
{
	check(MaxGrid > Index && ItemList[Index].Count >= Count);
	ItemList[Index].Count -= Count;
	if (ItemList[Index].Count <= 0)
	{
		ItemList[Index] = FBackpackItemInfo();
	}

	OnChanged.Broadcast();
}

int32 UBackpackComponent::CountItem(const FName& RowName) const
{
	int32 Count = 0;
	for (const FBackpackItemInfo& ItemInfo : ItemList)
	{
		if (ItemInfo.RowName == RowName)
		{
			Count += ItemInfo.Count;
		}
	}
	return Count;
}

FBackpackItemInfo UBackpackComponent::FindItem(int32 Index) const
{
	if (Index < MaxGrid)
	{
		return ItemList[Index];
	}
	return FBackpackItemInfo();
}

int32 UBackpackComponent::GetCurGrid() const
{
	int32 Num = 0;
	for (const FBackpackItemInfo& Info : ItemList)
	{
		if (Info.RowName != EMPTY_NAME)
		{
			Num += 1;
		}
	}
	return Num;
}

float UBackpackComponent::GetCurMass() const
{
	float Mass = 0.f;
	for (const FBackpackItemInfo& ItemInfo : ItemList)
	{
		const FBasicItemData& ItemData = UMainFunctionLibrary::GetBasicItemData(ItemInfo.RowName);
		Mass += ItemData.Mass * ItemInfo.Count;
	}
	return Mass;
}

void UBackpackComponent::Reorder()
{
}

