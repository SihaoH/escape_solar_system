// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlanetInfo.generated.h"

/**
 * 星球信息UI父类
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UPlanetInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetLooked(bool bLooked);
	void SetLocked(bool bLocked);
	void SetName(FText Name);
	void SetDistance(float Dist);
	void SetSpeed(float Speed);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnLooked(ESlateVisibility OutVisibility);
	UFUNCTION(BlueprintImplementableEvent)
	void OnLocked(ESlateVisibility OutVisibility);
	UFUNCTION(BlueprintImplementableEvent)
	void OnNameSetted(const FText& Name);
	UFUNCTION(BlueprintImplementableEvent)
	void OnDistanceChanged(const FText& Dist);
	UFUNCTION(BlueprintImplementableEvent)
	void OnSpeedChanged(const FText& Speed);

private:
	bool bSelfLocked = false;
};
