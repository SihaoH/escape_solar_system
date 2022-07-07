// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "TechData.h"
#include "ItemDataObject.h"
#include "CoreMinimal.h"
#include "MenuTech.generated.h"

/**
 * 菜单页面中的等级部件
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UMenuTechHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SelectTech(const TArray<ETech>& Props);

	UFUNCTION(BlueprintCallable)
	void UpgradeTech(const TArray<ETech>& Props);

	UFUNCTION(BlueprintPure)
	FText GetTechName(const TArray<ETech>& Props);
	UFUNCTION(BlueprintPure)
	FText GetTechDesc(const TArray<ETech>& Props);
	UFUNCTION(BlueprintPure)
	int32 GetMaxLv(const TArray<ETech>& Props);
	UFUNCTION(BlueprintPure)
	int32 GetCurLv(const TArray<ETech>& Props);


	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly))
	void Debug();

private:
	int* GetTarget(ETech Level);
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
