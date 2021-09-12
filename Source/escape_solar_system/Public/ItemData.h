﻿// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

/** 简化的整数范围结构 */
USTRUCT(BlueprintType)
struct FSInt32Range
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Min = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Max = 0;
};

/** 所有物品，RowName为0000~9999 */
USTRUCT(BlueprintType)
struct FBasicItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/** 物品名称 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	/** 质量，单位kg */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mass = 0.f;

	/** 最大堆叠数，-1为不限制 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStack = -1;

	/** 物品描述 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Desc;

	/** 物品图标 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;
};

/** 可拾取的物品 */
USTRUCT(BlueprintType)
struct FPickableItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/** 在关卡中重新生成的时间, 单位秒 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSInt32Range RebirthTime;
	
	/** 一次采集可获取的数量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSInt32Range PickCount;
};

/** 可合成的物品 */
USTRUCT(BlueprintType)
struct FMakeableItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/** 合成所需的物品清单 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> DemandList;
};