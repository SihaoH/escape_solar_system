// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "LevelData.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuLevel.generated.h"


/**
 * 菜单页面中的等级部件
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMenuLevel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SelectLevel(const TArray<ELevel>& Prop);

	UFUNCTION(BlueprintCallable)
	void UpgradeLevel(const TArray<ELevel>& Prop);

	UFUNCTION(BlueprintCallable)
	int32 GetMaxVal(const TArray<ELevel>& Prop);

	UFUNCTION(BlueprintCallable)
	int32 GetCurVal(const TArray<ELevel>& Prop);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnLevelUpgraded();

private:
	int* GetTarget(ELevel Level);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool CanUpgrade = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText CurLevelInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText NextLevelInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText DemandPoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText DemandItems;
};
