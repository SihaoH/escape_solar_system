// Copyright 2020 H₂S. All Rights Reserved.

#include "MainFunctionLibrary.h"
#include "BackpackComponent.h"
#include "MainLevelScriptActor.h"
#define LOCTEXT_NAMESPACE "MainFunctionLibrary"

UDataTable* UMainFunctionLibrary::DT_ItemInfo = nullptr;

UDataTable* UMainFunctionLibrary::DT_LevelDemand = nullptr;
UDataTable* UMainFunctionLibrary::DT_LevelValue = nullptr;

UMainFunctionLibrary::UMainFunctionLibrary()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Item(TEXT("DataTable'/Game/DataTable/DT_ItemInfo.DT_ItemInfo'"));
	DT_ItemInfo = Finder_Item.Object;
	check(DT_ItemInfo);

	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Demand(TEXT("DataTable'/Game/DataTable/DT_LevelDemand.DT_LevelDemand'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Value(TEXT("DataTable'/Game/DataTable/DT_LevelValue.DT_LevelValue'"));
	DT_LevelDemand = Finder_Demand.Object;
	check(DT_LevelDemand);
	DT_LevelValue = Finder_Value.Object;
	check(DT_LevelValue);
}

void UMainFunctionLibrary::SendMessage(FText msg)
{
	if (AMainLevelScriptActor::GetInstance())
	{
		AMainLevelScriptActor::GetInstance()->MessagedDelegate.Broadcast(msg);
	}
	// 其他Level的委派广播（如有）
}

FItemData& UMainFunctionLibrary::GetItemData(const FName& RowName)
{
	return *DT_ItemInfo->FindRow<FItemData>(RowName, FString());
}

TArray<FName> UMainFunctionLibrary::GetMakeableItemList()
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

FLevelDemand UMainFunctionLibrary::GetLevelDemand(ELevel Level, int32 Val)
{
	FLevelDemandList* DemandList = DT_LevelDemand->FindRow<FLevelDemandList>(LV::DemandRow[Level], FString());
	if (DemandList)
	{
		if (Val >= 0 && Val < DemandList->List.Num())
		{
			return DemandList->List[Val];
		}
	}
	//check(false);
	return FLevelDemand();
}

float UMainFunctionLibrary::GetLevelValue(ELevel Level, int32 Val)
{
	FLevelValueList* ValueList = DT_LevelValue->FindRow<FLevelValueList>(LV::ValueRow[Level], FString());
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

TPair<bool, FText> UMainFunctionLibrary::GetDemandInfo(const TMap<FName, int32>& List, UBackpackComponent* Backpack, int32 Count)
{
	FString DemandStr;
	bool Enough = true;
	for (const TPair<FName, int32>& Demand : List)
	{
		FItemData& DemandData = UMainFunctionLibrary::GetItemData(Demand.Key);
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
