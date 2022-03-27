// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MenuBase.generated.h"

/**
 * 菜单页面中的基地（制作）部件
 */
UCLASS(BlueprintType)
class ESCAPE_SOLAR_SYSTEM_API UMenuBaseHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	void GetMakeableList(TArray<class UItemDataObject*>& OutItems) const;
	UFUNCTION(BlueprintCallable)
	void OnItemSelectionChanged(UObject* SelItem, float& MakeableMin, float& MakeableMax);
	UFUNCTION(BlueprintCallable)
	void OnMakeCountChanged(UObject* SelItem, float Count, FText& DemandText);
	UFUNCTION(BlueprintCallable)
	void MakeItem(UObject* SelItem, float Count);

private:
	int32 GetMaxMakeableCount(const FName& RowName) const;
	FORCEINLINE class UBackpackComponent* GetStorehouse() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ESlateVisibility UnmakeableVisibility;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ESlateVisibility MakeableVisibility;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool CanMake = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FSlateBrush ItemIcon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemDesc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemMass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemCount;
};
