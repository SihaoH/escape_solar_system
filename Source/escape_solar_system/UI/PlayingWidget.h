// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayingWidget.generated.h"

/**
 * 游玩进行状态时显示的UI，包括基本的HP/MP条
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UPlayingHelper : public UObject
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintPure)
	void GetDebugInfo(TArray<FText>& OutList) const;
};
