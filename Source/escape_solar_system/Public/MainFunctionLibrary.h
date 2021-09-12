// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "ItemData.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MainFunctionLibrary.generated.h"

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMainFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UMainFunctionLibrary();

public:
	UFUNCTION(BlueprintCallable)
	static FBasicItemData& GetBasicItemData(const FName& RowName);

	UFUNCTION(BlueprintCallable)
	static TArray<FName> GetMakeableItemList();

	UFUNCTION(BlueprintCallable)
	static FMakeableItemData& GetMakeableItemData(const FName& RowName);

private:
	static UDataTable* BasicItemDataTable;
	static UDataTable* PickableItemDataTable;
	static UDataTable* MakeableItemDataTable;
};
