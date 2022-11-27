// Copyright 2020 H₂S. All Rights Reserved.

#include "MainLibrary.h"
#include "BackpackComponent.h"
#include "MainLevelScript.h"
#include <GameFramework/GameUserSettings.h>

UDataTable* UMainLibrary::DT_ItemInfo = nullptr;
UDataTable* UMainLibrary::DT_TechValue = nullptr;
TMap<FString, FText> UMainLibrary::TranslationMap;

UMainLibrary::UMainLibrary()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Item(TEXT("DataTable'/Game/DataTable/DT_ItemInfo.DT_ItemInfo'"));
	DT_ItemInfo = Finder_Item.Object;
	check(DT_ItemInfo);

	static ConstructorHelpers::FObjectFinder<UDataTable> Finder_Value(TEXT("DataTable'/Game/DataTable/DT_TechValue.DT_TechValue'"));
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
	if (FItemData* DataPtr = DT_ItemInfo->FindRow<FItemData>(RowName, FString()))
	{
		return *DataPtr;
	}
	else
	{
		ensure(false);
		static FItemData Tmp;
		return Tmp;
	}
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

FText UMainLibrary::GetTechValueName(ETech Tech)
{
	FTechValue* Value = DT_TechValue ? DT_TechValue->FindRow<FTechValue>(TECH::ValueRow[Tech], FString()) : nullptr;
	if (Value)
	{
		return Value->Name;
	}
	//check(false);
	return FText();
}

float UMainLibrary::GetTechValueVal(ETech Tech, int32 Val)
{
	FTechValue* Value = DT_TechValue ? DT_TechValue->FindRow<FTechValue>(TECH::ValueRow[Tech], FString()) : nullptr;
	if (Value)
	{
		if (Val >= 0 && Val < Value->List.Num())
		{
			return Value->List[Val];
		}
	}
	//check(false);
	return 0;
}
