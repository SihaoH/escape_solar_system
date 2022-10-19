// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayingView.generated.h"

/**
 * 游玩进行状态时显示的UI，例如按键提示
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UPlayingViewHelper : public UObject
{
	GENERATED_BODY()

public:
	UPlayingViewHelper();

	/** 获取Action操作所绑定的按键组 */
	UFUNCTION(BlueprintPure)
	TArray<FKey> GetActionKeys(FName ActionName);

	/** 获取Axis操作所绑定的按键组 */
	UFUNCTION(BlueprintPure)
	TArray<FKey> GetAxisKeys(FName AxisName);
};
