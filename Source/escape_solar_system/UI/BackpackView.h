// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackpackView.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemClickedDynamic, class UBackpackView*, BackpackView, UObject*, Item);

/**
 * 菜单背包部件的背包列表视图，包括背包名称、承重信息、物品列表
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UBackpackView : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetBackpack(class UBackpackComponent* Bp);
	void ClearSelection();

protected:
	virtual void NativePreConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	UFUNCTION()
	void OnBackpackChanged();

	UFUNCTION()
	void OnTileViewItemClicked(UObject* Item);

public:
	FOnItemClickedDynamic OnItemClicked;

private:
	class UBackpackComponent* BpComp;
	class UTileView* TileView_Item = nullptr;
	class UTextBlock* TextBlock_Bearing = nullptr;
};
