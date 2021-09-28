// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuBackpack.generated.h"

class UBackpackView;
class UBackpackComponent;

/**
 * 菜单页面中的背包部件
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMenuBackpack : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitBpView(UBackpackView* Body, UBackpackView* Base, UBackpackView* Ship);
	UFUNCTION(BlueprintCallable)
	void AcceptDrop(float Count);
	UFUNCTION(BlueprintCallable)
	void DiscardItem(float Count);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnItemSelected(float Max);

	UFUNCTION(BlueprintImplementableEvent)
	void OnItemDrop(float MaxAdd);

private:
	void CheckBpView();
	void SelectItem(class UItemDataObject* Item);

	UFUNCTION()
	void OnBpViewItemClicked(UObject* Item);
	UFUNCTION()
	void OnBpViewItemDrop(UBackpackComponent* DstBp, UBackpackComponent* SrcBp, FName RowName);

private:
	UBackpackView* BpView_Body = nullptr;
	UBackpackView* BpView_Base = nullptr;
	UBackpackView* BpView_Ship = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ESlateVisibility NoneVisibility;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ESlateVisibility ItemVisibility;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FSlateBrush ItemIcon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemDesc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemMass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemCount;

	// 准备转移物品的相关对象：目标背包、源背包、源物品
	class UBackpackComponent* DstBackpack = nullptr;
	class UBackpackComponent* SrcBackpack = nullptr;
	FName SrcItem = NAME_None;

	// 当前选中的物品
	class UItemDataObject* SelectedItem = nullptr;
};
