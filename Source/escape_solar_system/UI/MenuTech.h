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
	TPair<bool, FText> GetDemandInfo(const TMap<FName, int32>& List, class UBackpackComponent* Backpack = nullptr, int32 Count = 1);
	inline TArray<class UBackpackComponent*> GetBackpackList();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool CanUpgrade = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText DemandPoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText DemandItems;

private:
	TObjectPtr<UDataTable> DT_TechInfo;
};
