// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackpackView.generated.h"

class UBackpackComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemDropDynamic, UBackpackComponent*, DstBp, UBackpackComponent*, SrcBp, FName, RowName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemClickedDynamic, UObject*, Item);

/**
 * 菜单背包部件的背包列表视图，包括背包名称、承重信息、物品列表
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UBackpackView : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetBackpack(UBackpackComponent* Bp);

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnListViewResetted(const TArray<class UItemDataObject*>& OutItems);

	UFUNCTION(BlueprintCallable)
	void InitEntryWidget(UUserWidget* Entry);

	UFUNCTION(BlueprintCallable)
	void ClickItem(UObject* Item);

private:
	UFUNCTION()
	void OnBackpackChanged();

public:
	FOnItemDropDynamic OnItemDrop;
	FOnItemClickedDynamic OnEntryClicked;
	FOnItemClickedDynamic OnItemClicked;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText InfoBearing;

	UBackpackComponent* Backpack = nullptr;
};
