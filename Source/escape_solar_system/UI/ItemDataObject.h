// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemDataObject.generated.h"

UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UItemDataObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = ItemDataObject, EditAnywhere, BlueprintReadWrite)
	FName RowName = NAME_None;

	UPROPERTY(Category = ItemDataObject, EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	UPROPERTY(Category = ItemDataObject, EditAnywhere, BlueprintReadWrite)
	class UBackpackComponent* Owner = nullptr;
};
