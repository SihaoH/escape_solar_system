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
};
