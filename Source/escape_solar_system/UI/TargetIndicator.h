// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetIndicator.generated.h"

/**
 * 星球信息Helper类
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UTargetIndicatorHelper : public UObject
{
	GENERATED_BODY()

public:
	/** 获取锁定星球的距离和相对速度，距离单位都是m */
	UFUNCTION(BlueprintPure)
	void GetLockInfo(const class AActor* Target, float& Dist, float& Speed);

	UFUNCTION(BlueprintPure)
	void GetWidgetPosition(const class AActor* Target, FVector2D& Position, int32& Angle);

private:
	float LastDist = 0.f;
};
