// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MainFunctionLibrary.h"
#include "CoreMinimal.h"
#include "MenuBackpack.generated.h"

class UBackpackViewHelper;
class UBackpackComponent;

/* 可对物品进行的操作 */
UENUM(BlueprintType)
enum class EItemOptions : uint8
{
	Use,
	ConsumeToChar,
	ConsumeToShip,
	Discard
};

/**
 * 菜单页面中的背包部件
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UMenuBackpackHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	void GetListViewItems(UBackpackComponent* InBp, TArray<class UItemDataObject*>& OutItems) const;

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
	void TryDropItem(UBackpackComponent* DstBp, UBackpackComponent* SrcBp, bool& NeedHint, int32& MaxAdd, FText& Reason);

	UFUNCTION(BlueprintPure)
	void GetMenuDisplay(TMap<EItemOptions, FText>& OutList);
	UFUNCTION(BlueprintPure)
	void GetMenuOptions(TArray<EItemOptions>& OutList);

private:
	// 准备转移物品的相关对象：目标背包、源背包
	class UBackpackComponent* DstBackpack = nullptr;
	class UBackpackComponent* SrcBackpack = nullptr;

	// 当前选中的物品
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UItemDataObject* SelectedItem = nullptr;
};
