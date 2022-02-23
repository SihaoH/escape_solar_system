// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayingWidget.generated.h"

/**
 * 游玩进行状态时显示的UI，包括基本的HP/MP条
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UPlayingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void UpdateDebugInfo(TArray<FText>& OutList);
};
