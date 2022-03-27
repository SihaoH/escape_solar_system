// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackpackView.generated.h"

UENUM(BlueprintType)
enum class EBackpackType : uint8
{
	Char,
	Ship,
	Base
};

class UBackpackComponent;

/**
 * 菜单背包部件的背包列表视图，包括背包名称、承重信息、物品列表
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UBackpackViewHelper : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	UBackpackComponent* GetBackpack() const;

	UFUNCTION(BlueprintPure)
	void GetListViewItems(TArray<class UItemDataObject*>& OutItems) const;

	UFUNCTION(BlueprintPure)
	void GetBearingInfo(float& CurMass, float& MaxBearing) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText InfoBearing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EBackpackType Type;
};
