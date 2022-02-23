// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuBase.generated.h"

/**
 * 菜单页面中的基地（制作）部件
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMenuBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitMakeableListView(class UListView* ListView);
	UFUNCTION(BlueprintCallable)
	void OnItemSelectionChanged(UObject* Item, float& MakeableMin, float& MakeableMax);
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
