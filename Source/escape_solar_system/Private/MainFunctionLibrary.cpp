// Copyright 2020 H₂S. All Rights Reserved.

#include "MainFunctionLibrary.h"

UDataTable* UMainFunctionLibrary::BasicItemDataTable = nullptr;
UDataTable* UMainFunctionLibrary::PickableItemDataTable = nullptr;
UDataTable* UMainFunctionLibrary::MakeableItemDataTable = nullptr;

UMainFunctionLibrary::UMainFunctionLibrary()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> BasicFinder(TEXT("DataTable'/Game/Item/BasicItemDataTable.BasicItemDataTable'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> PickableFinder(TEXT("DataTable'/Game/Item/PickableItemDataTable.PickableItemDataTable'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> MakeableFinder(TEXT("DataTable'/Game/Item/MakeableItemDataTable.MakeableItemDataTable'"));
	BasicItemDataTable = BasicFinder.Object;
	PickableItemDataTable = PickableFinder.Object;
	MakeableItemDataTable = MakeableFinder.Object;
	check(BasicItemDataTable);
	check(PickableItemDataTable);
	check(MakeableItemDataTable);
}

FBasicItemData& UMainFunctionLibrary::GetBasicItemData(const FName& RowName)
{
	return *BasicItemDataTable->FindRow<FBasicItemData>(RowName, FString());
}

FPickableItemData& UMainFunctionLibrary::GetPickableItemData(const FName& RowName)
{
	return *PickableItemDataTable->FindRow<FPickableItemData>(RowName, FString());
}

TArray<FName> UMainFunctionLibrary::GetMakeableItemList()
{
	return MakeableItemDataTable->GetRowNames();
}

FMakeableItemData& UMainFunctionLibrary::GetMakeableItemData(const FName& RowName)
{
	return *MakeableItemDataTable->FindRow<FMakeableItemData>(RowName, FString());
}
