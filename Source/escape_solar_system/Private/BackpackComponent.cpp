﻿// Copyright 2020 H₂S. All Rights Reserved.

#include "BackpackComponent.h"
#include "MainFunctionLibrary.h"

UBackpackComponent::UBackpackComponent()
{

}

void UBackpackComponent::AddItem(const FName& RowName, int32 Count)
{
	const FBasicItemData& ItemData = UMainFunctionLibrary::GetBasicItemData(RowName);
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

	OnChanged.Broadcast();
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

	OnChanged.Broadcast();
}

int32 UBackpackComponent::GetMaxAddNum(const FName& RowName)
{
	if (MaxBearing < 0)
	{
		return MAX_int32;
	}
	const FBasicItemData& ItemData = UMainFunctionLibrary::GetBasicItemData(RowName);
	float Remaining = MaxBearing - GetCurMass();
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

float UBackpackComponent::GetCurMass() const
{
	float Mass = 0.f;
	for (const auto& Elem : ItemList)
	{
		const FBasicItemData& ItemData = UMainFunctionLibrary::GetBasicItemData(Elem.Key);
		Mass += ItemData.Mass * Elem.Value;
	}
	return Mass;
}

