// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "ItemData.h"
#include "LevelData.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MainFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EPawnType : uint8
{
	MainChar,
	SpaceShip
};

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMainFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UMainFunctionLibrary();

public:
	static FItemData& GetItemData(const FName& RowName);
	static TArray<FName> GetMakeableItemList();

	static FLevelDemand GetLevelDemand(ELevel Level, int32 Val);
	static float GetLevelValue(ELevel Level, int32 Val);

	static TPair<bool, FText> GetDemandInfo(const TMap<FName, int32>& List, class UBackpackComponent* Backpack = nullptr, int32 Count = 1);

private:
	static UDataTable* DT_ItemInfo;
	static UDataTable* DT_LevelDemand;
	static UDataTable* DT_LevelValue;
};
