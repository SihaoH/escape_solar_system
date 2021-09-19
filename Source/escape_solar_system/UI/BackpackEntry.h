// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "BackpackEntry.generated.h"

/**
 * 背包列表的条目
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UBackpackEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnEntryClicked(UObject* Item);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	//virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

private:
	void SetSelectedStyle(bool bIsSelected);

private:
	class UImage* Image_Icon = nullptr;
	class UImage* Image_Border = nullptr;
	class UTextBlock* TextBlock_Count = nullptr;

	UObject* SelfItem = nullptr;
	static UObject* SelectedItem;
};
