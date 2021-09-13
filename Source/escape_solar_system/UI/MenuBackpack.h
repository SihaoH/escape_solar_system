// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuBackpack.generated.h"

/**
 * 菜单页面中的背包部件
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMenuBackpack : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnBpViewItemClicked(class UBackpackView* BpView, UObject* Item);

	void SelectItem(class UItemDataObject* Item);

private:
	class UBackpackView* BpView_Body = nullptr;
	class UBackpackView* BpView_Base = nullptr;
	class UBackpackView* BpView_Ship = nullptr;

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
	FText ItemStack;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemCount;
};
