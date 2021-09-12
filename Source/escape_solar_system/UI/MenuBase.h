// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuBase.generated.h"

/**
 * 菜单页面中的基地（制作）部件
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMenuBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	int32 GetMaxMakeableCount(const FName& RowName);

private:
	void OnItemSelectionChanged(UObject*Item);
	UFUNCTION()
	void OnMakeClicked();
	UFUNCTION()
	void OnMakeCountChanged(float Count);

private:
	class UListView* ListView_Makeable = nullptr;
	class UUserWidget* WB_NumberEditor = nullptr;
	class UButton* Button_Make = nullptr;
	class UImage* Image_Item = nullptr;
	class UTextBlock* TextBlock_Name = nullptr;
	class UTextBlock* TextBlock_Owned = nullptr;
	class UTextBlock* TextBlock_Desc = nullptr;
	class UTextBlock* TextBlock_Demand = nullptr;
};
