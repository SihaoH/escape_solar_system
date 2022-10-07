// Copyright 2020 H₂S. All Rights Reserved.

#include "MainLibrary.h"
#include "BackpackComponent.h"
#include "MainLevelScript.h"

UDataTable* UMainLibrary::DT_ItemInfo = nullptr;

UDataTable* UMainLibrary::DT_TechDemand = nullptr;
UDataTable* UMainLibrary::DT_TechValue = nullptr;
TMap<FString, FText> UMainLibrary::TranslationMap;

UMainLibrary::UMainLibrary()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Item(TEXT("DataTable'/Game/DataTable/DT_ItemInfo.DT_ItemInfo'"));
	DT_ItemInfo = Finder_Item.Object;
	check(DT_ItemInfo);

	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Demand(TEXT("DataTable'/Game/DataTable/DT_TechDemand.DT_TechDemand'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Value(TEXT("DataTable'/Game/DataTable/DT_TechValue.DT_TechValue'"));
	DT_TechDemand = Finder_Demand.Object;
	check(DT_TechDemand);
	DT_TechValue = Finder_Value.Object;
	check(DT_TechValue);

	ConstructorHelpers::FObjectFinder<UDataTable> Finder_Tr(TEXT("DataTable'/Game/DataTable/DT_Tr.DT_Tr'"));
	auto DT_Tr = Finder_Tr.Object;
	if (DT_Tr)
	{
		TArray<FTranslatedData*> TrList;
		DT_Tr->GetAllRows<FTranslatedData>(FString(), TrList);
		for (auto Item : TrList)
		{
			TranslationMap.Add(Item->Source, Item->Text);
		}
	}
}

FText UMainLibrary::Translate(const FString& Str)
{
	if (TranslationMap.Contains(Str))
	{
		return TranslationMap[Str];
	}
	ensure(false);
	return FText::FromString(Str);
}

void UMainLibrary::SendMessage(FText Msg)
{
	if (AMainLevelScript::Instance())
	{
		AMainLevelScript::Instance()->MessagedDelegate.Broadcast(Msg);
	}
	// 其他Level的委派广播（如有）
}

FItemData& UMainLibrary::GetItemData(const FName& RowName)
{
	return *DT_ItemInfo->FindRow<FItemData>(RowName, FString());
}

TArray<FName> UMainLibrary::GetMakeableItemList()
{
	static TArray<FName> List;
	if (List.Num() <= 0)
	{
		for (const FName& Name : DT_ItemInfo->GetRowNames())
		{
			if (GetItemData(Name).DemandList.Num() > 0)
			{
				List.Add(Name);
			}
		}
	}
	return List;
}

FTechDemand UMainLibrary::GetTechDemand(ETech Level, int32 Val)
{
	FTechDemandList* DemandList = DT_TechDemand ? DT_TechDemand->FindRow<FTechDemandList>(TECH::DemandRow[Level], FString()) : nullptr;
	if (DemandList)
	{
		if (Val >= 0 && Val < DemandList->List.Num())
		{
			return DemandList->List[Val];
		}
	}
	//check(false);
	return FTechDemand();
}

float UMainLibrary::GetTechValue(ETech Level, int32 Val)
{
	FTechValueList* ValueList = DT_TechValue ? DT_TechValue->FindRow<FTechValueList>(TECH::ValueRow[Level], FString()) : nullptr;
	if (ValueList)
	{
		if (Val >= 0 && Val < ValueList->List.Num())
		{
			return ValueList->List[Val];
		}
	}
	//check(false);
	return 0;
}

TPair<bool, FText> UMainLibrary::GetDemandInfo(const TMap<FName, int32>& List, UBackpackComponent* Backpack, int32 Count)
{
	FString DemandStr;
	bool Enough = true;
	for (const TPair<FName, int32>& Demand : List)
	{
		FItemData& DemandData = UMainLibrary::GetItemData(Demand.Key);
		int32 NeedCount = Demand.Value * FMath::Max(Count, 1);
		int32 HoldCount = Backpack ? Backpack->CountItem(Demand.Key) : 0;
		FStringFormatOrderedArguments Arguments;
		Arguments.Add(DemandData.Name.ToString());
		Arguments.Add(HoldCount >= NeedCount ? TEXT("Default") : TEXT("Warning"));
		Arguments.Add(FString::FromInt(NeedCount));
		Arguments.Add(Backpack ? FString::FromInt(HoldCount) : tr("？？？").ToString());
		DemandStr += FString::Format(TEXT("{0}  <{1}>×{2}</>  (<img id=\"Storehouse\"/> ×{3})\n"), Arguments);
		if (HoldCount < NeedCount)
		{
			Enough = false;
		}
	}
	DemandStr.RemoveFromEnd(TEXT("\n"));
	return TPair<bool, FText>(Enough, FText::FromString(DemandStr));
}
