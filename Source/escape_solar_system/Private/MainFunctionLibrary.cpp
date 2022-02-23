// Copyright 2020 H₂S. All Rights Reserved.

#include "MainFunctionLibrary.h"
#include "BackpackComponent.h"
#define LOCTEXT_NAMESPACE "MainFunctionLibrary"

UDataTable* UMainFunctionLibrary::DT_BasicItem = nullptr;
UDataTable* UMainFunctionLibrary::DT_PickableItem = nullptr;
UDataTable* UMainFunctionLibrary::DT_MakeableItem = nullptr;

UDataTable* UMainFunctionLibrary::DT_LevelDemand = nullptr;
UDataTable* UMainFunctionLibrary::DT_LevelValue = nullptr;

UMainFunctionLibrary::UMainFunctionLibrary()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Basic(TEXT("DataTable'/Game/DataTable/DT_BasicItem.DT_BasicItem'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Pickable(TEXT("DataTable'/Game/DataTable/DT_PickableItem.DT_PickableItem'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Makeable(TEXT("DataTable'/Game/DataTable/DT_MakeableItem.DT_MakeableItem'"));
	DT_BasicItem = Finder_Basic.Object;
	DT_PickableItem = Finder_Pickable.Object;
	DT_MakeableItem = Finder_Makeable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Demand(TEXT("DataTable'/Game/DataTable/DT_LevelDemand.DT_LevelDemand'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Value(TEXT("DataTable'/Game/DataTable/DT_LevelValue.DT_LevelValue'"));
	DT_LevelDemand = Finder_Demand.Object;
	DT_LevelValue = Finder_Value.Object;
}

FBasicItemData& UMainFunctionLibrary::GetBasicItemData(const FName& RowName)
{
	return *DT_BasicItem->FindRow<FBasicItemData>(RowName, FString());
}

FPickableItemData& UMainFunctionLibrary::GetPickableItemData(const FName& RowName)
{
	return *DT_PickableItem->FindRow<FPickableItemData>(RowName, FString());
}

TArray<FName> UMainFunctionLibrary::GetMakeableItemList()
{
	return DT_MakeableItem->GetRowNames();
}

FMakeableItemData& UMainFunctionLibrary::GetMakeableItemData(const FName& RowName)
{
	return *DT_MakeableItem->FindRow<FMakeableItemData>(RowName, FString());
}

FLevelDemand UMainFunctionLibrary::GetLevelDemand(ELevel Level, int32 Val)
{
	FName RowName;
	switch (Level)
	{
	case ELevel::CharHP:
	case ELevel::CharMass:
		RowName = FName("CharStrength");
		break;
	default:
		RowName = LV::RowName[Level];
		break;
	}
	FLevelDemandList* DemandList = DT_LevelDemand->FindRow<FLevelDemandList>(RowName, FString());
	if (DemandList)
	{
		if (Val >= 0 && Val < DemandList->List.Num())
		{
			return DemandList->List[Val];
		}
	}
	check(false);
	return FLevelDemand();
}

float UMainFunctionLibrary::GetLevelValue(ELevel Level, int32 Val)
{
	FLevelValueList* ValueList = DT_LevelValue->FindRow<FLevelValueList>(LV::RowName[Level], FString());
	if (ValueList)
	{
		if (Val >= 0 && Val < ValueList->List.Num())
		{
			return ValueList->List[Val];
		}
	}
	check(false);
	return 0;
}

TPair<bool, FText> UMainFunctionLibrary::GetDemandInfo(const TMap<FName, int32>& List, UBackpackComponent* Backpack, int32 Count)
{
	FString DemandStr;
	bool Enough = true;
	for (const TPair<FName, int32>& Demand : List)
	{
		FBasicItemData& DemandData = UMainFunctionLibrary::GetBasicItemData(Demand.Key);
		int32 NeedCount = Demand.Value * FMath::Max(Count, 1);
		int32 HoldCount = Backpack ? Backpack->CountItem(Demand.Key) : 0;
		FStringFormatOrderedArguments Arguments;
		Arguments.Add(DemandData.Name.ToString());
		Arguments.Add(HoldCount >= NeedCount ? TEXT("Default") : TEXT("Warning"));
		Arguments.Add(FString::FromInt(NeedCount));
		Arguments.Add(Backpack ? FString::FromInt(HoldCount) : LOCTEXT("unknow", "？？？").ToString());
		DemandStr += FString::Format(TEXT("{0}  <{1}>x{2}</>  (<img id=\"Storehouse\"/> x{3})\n"), Arguments);
		if (HoldCount < NeedCount)
		{
			Enough = false;
		}
	}
	return TPair<bool, FText>(Enough, FText::FromString(DemandStr));
}

#undef LOCTEXT_NAMESPACE
