// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackpackView.generated.h"

class UBackpackComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemDropDynamic, UBackpackComponent*, DstBp, UBackpackComponent*, SrcBp, int32, SrcIdx);
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

	UFUNCTION(BlueprintCallable)
	void OnEntryWidgetInited(UUserWidget* Entry);

protected:
	virtual void NativePreConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	UFUNCTION()
	void OnBackpackChanged();

	UFUNCTION()
	void OnTileViewItemClicked(UObject* Item);

public:
	FOnItemDropDynamic OnItemDrop;
	FOnItemClickedDynamic OnItemClicked;
	FOnItemClickedDynamic OnEntryClicked;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText InfoBearing;

	UBackpackComponent* Backpack = nullptr;
	class UTileView* TileView_Item = nullptr;
};
