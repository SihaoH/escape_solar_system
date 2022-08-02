﻿// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "ItemData.h"
#include "TechData.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MainLibrary.generated.h"

UENUM(BlueprintType)
enum class EPawnType : uint8
{
	MainChar,
	SpaceShip
};

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMainLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UMainLibrary();

public:
	UFUNCTION(BlueprintPure)
	static FORCEINLINE bool IsDev()
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		return true;
#endif
		return false;
	}

	UFUNCTION(BlueprintCallable)
	static void SendMessage(FText Msg);

	UFUNCTION(BlueprintCallable)
	static FItemData& GetItemData(const FName& RowName);
	static TArray<FName> GetMakeableItemList();

	static FTechDemand GetTechDemand(ETech Level, int32 Val);
	static float GetTechValue(ETech Level, int32 Val);

	static TPair<bool, FText> GetDemandInfo(const TMap<FName, int32>& List, class UBackpackComponent* Backpack = nullptr, int32 Count = 1);

private:
	static UDataTable* DT_ItemInfo;
	static UDataTable* DT_TechDemand;
	static UDataTable* DT_TechValue;
};