// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "TechData.h"
#include "ItemDataObject.h"
#include "CoreMinimal.h"
#include "MenuTech.generated.h"

USTRUCT(BlueprintType)
struct FTechUpgradeDemand
{
	GENERATED_USTRUCT_BODY()

	FTechUpgradeDemand() = default;

	FTechUpgradeDemand(FText T, int32 H, int32 N) : Name(T), Has(H), Need(N) {};

	/** 升级所需物品的名称 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	/** 升级所需物品的库存量 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Has;

	/** 升级所需物品的需求量 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Need;
};

/**
 * 菜单页面中的等级部件
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UMenuTechHelper : public UObject
{
	GENERATED_BODY()

public:
	UMenuTechHelper();

	UFUNCTION(BlueprintCallable)
	void SelectTech(const TArray<ETech>& Props);

	UFUNCTION(BlueprintCallable)
	void UpgradeTech(const TArray<ETech>& Props);

	UFUNCTION(BlueprintPure)
	FText GetTechName(const TArray<ETech>& Props);
	UFUNCTION(BlueprintPure)
	FText GetTechDesc(const TArray<ETech>& Props);
	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<UTexture2D> GetTechIcon(const TArray<ETech>& Props);
	UFUNCTION(BlueprintPure)
	int32 GetMaxLv(const TArray<ETech>& Props);
	UFUNCTION(BlueprintPure)
	int32 GetCurLv(const TArray<ETech>& Props);
	UFUNCTION(BlueprintPure)
	FText GetTechValueName(ETech Prop);
	UFUNCTION(BlueprintPure)
	float GetTechValueVal(ETech Prop, bool bNext = false);

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly))
	void Debug();

private:
	int* GetTarget(ETech Tech);
	FTechDemand GetTechDemand(ETech Tech, int32 Val);
	bool GetDemandInfo(const TMap<FName, int32>& List, class UBackpackComponent* Backpack = nullptr);
	inline TArray<class UBackpackComponent*> GetBackpackList();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool CanUpgrade = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 DemandPoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FTechUpgradeDemand> DemandItems;

private:
	TObjectPtr<UDataTable> DT_TechInfo;
};
