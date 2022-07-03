// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayingWidget.generated.h"

/**
 * 游玩进行状态时显示的UI，例如按键提示
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UPlayingHelper : public UObject
{
	GENERATED_BODY()

public:
	UPlayingHelper();

	/** 获取Action操作所绑定的按键组 */
	UFUNCTION(BlueprintPure)
	TArray<FKey> GetActionKeys(FName ActionName);

	/** 获取Axis操作所绑定的按键组 */
	UFUNCTION(BlueprintPure)
	TArray<FKey> GetAxisKeys(FName AxisName);

	/** 获取主角的操作提示文本 */
	UFUNCTION(BlueprintPure)
	FText GetCharOperDesc(FName Name);

	/** 获取飞船的操作提示文本 */
	UFUNCTION(BlueprintPure)
	FText GetShipOperDesc(FName Name);

protected:
	UFUNCTION(BlueprintPure)
	void GetDebugInfo(TArray<FText>& OutList) const;

private:
	TMap<FName, FText> CharOperDesc;
	TMap<FName, FText> ShipOperDesc;
};
