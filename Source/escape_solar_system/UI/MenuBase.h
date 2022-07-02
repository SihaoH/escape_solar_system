// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MenuBase.generated.h"

/**
 * 菜单页面中的基地（制作）部件
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UMenuBaseHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	float GetShipDistance() const;
	UFUNCTION(BlueprintPure)
	void GetMakeableList(TArray<class UItemDataObject*>& OutItems) const;
	UFUNCTION(BlueprintPure)
	int32 GetMaxMakeableCount(const FName& RowName) const;
	UFUNCTION(BlueprintPure)
	int32 GetHoldCount(const FName& RowName) const;
	UFUNCTION(BlueprintCallable)
	void MakeItem(UObject* SelItem, float Count);

private:
	FORCEINLINE class UBackpackComponent* GetStorehouse() const;
};
