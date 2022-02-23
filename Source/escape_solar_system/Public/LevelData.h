// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LevelData.generated.h"


UENUM(BlueprintType)
enum class ELevel : uint8
{
	None,
	CharHP,
	CharMass,
	CharBackpack
};

namespace LV
{
const TMap<ELevel, FName> RowName = {
	{ELevel::CharHP, "CharHP"},
	{ELevel::CharMass, "CharMass"},
	{ELevel::CharBackpack, "CharBackpack"},
};

#define LOCTEXT_NAMESPACE "LevelData"
const TMap<ELevel, FText> DispName = {
	{ELevel::CharHP, LOCTEXT("HP", "血量")},
	{ELevel::CharMass, LOCTEXT("Mass", "质量")},
	{ELevel::CharBackpack, LOCTEXT("Backpack", "承重")},
};
#undef LOCTEXT_NAMESPACE
}


USTRUCT(BlueprintType)
struct FLevelDemand
{
	GENERATED_USTRUCT_BODY()

	/** 升级所需的点数 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Points = 0;

	/** 升级所需的物品 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, int32> Items;
};

USTRUCT(BlueprintType)
struct FLevelDemandList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLevelDemand> List = { FLevelDemand(), FLevelDemand(), FLevelDemand(), FLevelDemand(), FLevelDemand() };
};

USTRUCT(BlueprintType)
struct FLevelValueList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> List = {0, 0, 0, 0, 0};
};
