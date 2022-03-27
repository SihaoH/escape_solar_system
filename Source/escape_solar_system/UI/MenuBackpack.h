// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MainFunctionLibrary.h"
#include "CoreMinimal.h"
#include "MenuBackpack.generated.h"

class UBackpackViewHelper;
class UBackpackComponent;

/**
 * 菜单页面中的背包部件
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UMenuBackpackHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SelectItem(UObject* Item);
	UFUNCTION(BlueprintCallable)
	void UseItem();
	UFUNCTION(BlueprintCallable)
	void DropItem(int32 Count);
	UFUNCTION(BlueprintCallable)
	void ConsumeItem(int32 Count, EPawnType Target);
	UFUNCTION(BlueprintCallable)
	void DiscardItem(int32 Count);
	UFUNCTION(BlueprintCallable)
	void TryDropItem(UBackpackComponent* DstBp, UBackpackComponent* SrcBp, UObject* Item);

	UFUNCTION(BlueprintPure)
	int32 GetSelectedCount() const;
	UFUNCTION(BlueprintPure)
	void GetMenuStatus(bool& CanUse, bool& CanConsume);

private:
	// 准备转移物品的相关对象：目标背包、源背包
	class UBackpackComponent* DstBackpack = nullptr;
	class UBackpackComponent* SrcBackpack = nullptr;

	// 当前选中的物品
	class UItemDataObject* SelectedItem = nullptr;
};
