// Copyright 2020 H₂S. All Rights Reserved.

#include "BackpackView.h"
#include "BackpackComponent.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "ItemDataObject.h"

UBackpackComponent* UBackpackViewHelper::GetBackpack() const
{
	UBackpackComponent* Bp = nullptr;
	AMainCharacter* Char = AMainLevelScript::GetMainChar();
	if (Type == EBackpackType::Char)
	{
		Bp = Char->Backpack;
	}
	else if (Type == EBackpackType::Ship)
	{
		Bp = Char->FindSpaceship() ? Char->FindSpaceship()->Backpack : nullptr;
	}
	else if (Type == EBackpackType::Base)
	{
		Bp = Char->FindEarthBase() ? Char->FindEarthBase()->Backpack : nullptr;
	}
	return Bp;
}

void UBackpackViewHelper::GetListViewItems(TArray<class UItemDataObject*>& OutItems) const
{
	UBackpackComponent* Bp = GetBackpack();
	check(Bp);

	const auto& ItemList = Bp->GetItemList();
	for (const auto& Elem : ItemList)
	{
		UItemDataObject* Obj = NewObject<UItemDataObject>(const_cast<UBackpackViewHelper*>(this));
		Obj->RowName = Elem.Key;
		Obj->Count = Elem.Value;
		Obj->Owner = Bp;
		OutItems.Add(Obj);
	}
}

void UBackpackViewHelper::GetBearingInfo(float& CurMass, float& MaxBearing) const
{
	UBackpackComponent* Bp = GetBackpack();
	check(Bp);
	CurMass = Bp->GetMass();
	MaxBearing = Bp->GetMaxLoad();
}
