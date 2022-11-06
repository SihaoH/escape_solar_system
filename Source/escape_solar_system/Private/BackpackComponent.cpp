// Copyright 2020 H₂S. All Rights Reserved.

#include "BackpackComponent.h"

UBackpackComponent::UBackpackComponent()
{

}

void UBackpackComponent::SetBackpack(EPawnType Type, int32 Level)
{
	MaxLoad = UMainLibrary::GetTechValueVal(Type == EPawnType::MainChar ? ETech::CharBackpack : ETech::ShipBackpack, Level);
}

void UBackpackComponent::AddItem(const FName& RowName, int32 Count)
{
	if (Count <= 0) return;

	const FItemData& ItemData = UMainLibrary::GetItemData(RowName);
	check(&ItemData);
	check(Count <= GetMaxAddNum(RowName))

	if (ItemData.CanStack)
	{
		ItemList.FindOrAdd(RowName) += Count;
	}
	else
	{
		for (int32 i=0; i<Count; i++)
		{
			ItemList.Add(RowName, 1);
		}
	}

	UpdateMass();
	ChangedDelegate.Broadcast();
}

void UBackpackComponent::RemoveItem(const FName& RowName, int32 Count)
{
	int32* ValuePtr = ItemList.Find(RowName);
	check(ValuePtr);
	*ValuePtr -= Count;
	if (*ValuePtr <= 0)
	{
		ItemList.Remove(RowName, 0);
	}

	UpdateMass();
	ChangedDelegate.Broadcast();
}

int32 UBackpackComponent::GetMaxAddNum(const FName& RowName) const
{
	if (MaxLoad < 0)
	{
		return MAX_int32;
	}
	const FItemData& ItemData = UMainLibrary::GetItemData(RowName);
	float Remaining = MaxLoad - GetMass();
	return FMath::FloorToInt(Remaining / ItemData.Mass);
}

int32 UBackpackComponent::CountItem(const FName& RowName) const
{
	int32 Count = 0;
	for (const auto& Elem : ItemList)
	{
		if (Elem.Key == RowName)
		{
			Count += Elem.Value;
		}
	}
	return Count;
}

int32 UBackpackComponent::GetCurCount() const
{
	return ItemList.Num();
}

void UBackpackComponent::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving())
	{
		for (const auto& Pair : ItemList)
		{
			SavedList.Add(FItemPair(Pair.Key, Pair.Value));
		}
		Super::Serialize(Ar);
		SavedList.Empty(); // 存完之后就清空
	}
	else if (Ar.IsLoading())
	{
		Super::Serialize(Ar);
		for (const auto& Pair : SavedList)
		{
			ItemList.Add(Pair.Key, Pair.Value);
		}
		SavedList.Empty();
		UpdateMass();
	}
	else
	{
		Super::Serialize(Ar);
	}
}

void UBackpackComponent::UpdateMass()
{
	Mass = 0.f;
	for (const auto& Elem : ItemList)
	{
		const FItemData& ItemData = UMainLibrary::GetItemData(Elem.Key);
		Mass += ItemData.Mass * Elem.Value;
	}
}

