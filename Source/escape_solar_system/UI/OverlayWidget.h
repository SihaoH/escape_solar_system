// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverlayWidget.generated.h"

/**
 * 覆盖层的UI基类，打开时显示并移动光标在中心，按ESC关闭等统一处理；子类有暂停界面、菜单界面等
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(Category = OverlayWidget, BlueprintCallable)
	void Close();
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
