// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "LevelData.h"
#include "ItemDataObject.h"
#include "CoreMinimal.h"
#include "MenuLevel.generated.h"

/**
 * 菜单页面中的等级部件
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UMenuLevelHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SelectLevel(const TArray<ELevel>& Props);

	UFUNCTION(BlueprintCallable)
	void UpgradeLevel(const TArray<ELevel>& Props);

	UFUNCTION(BlueprintPure)
	FText GetLevelName(const TArray<ELevel>& Props);
	UFUNCTION(BlueprintPure)
	FText GetLevelDesc(const TArray<ELevel>& Props);
	UFUNCTION(BlueprintPure)
	int32 GetMaxVal(const TArray<ELevel>& Props);
	UFUNCTION(BlueprintPure)
	int32 GetCurVal(const TArray<ELevel>& Props);


	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly))
	void Debug();

private:
	int* GetTarget(ELevel Level);
	inline TArray<class UBackpackComponent*> GetBackpackList();

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
