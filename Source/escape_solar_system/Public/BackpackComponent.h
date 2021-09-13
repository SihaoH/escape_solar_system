// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/ObjectMacros.h"
#include "BackpackComponent.generated.h"

#define EMPTY_NAME FName(TEXT("-1"))

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackpackChangedEvent);

USTRUCT(BlueprintType)
struct FBackpackItemInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName RowName = EMPTY_NAME;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Count = 0;
};

/**
 * 背包组件
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UBackpackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBackpackComponent();

	virtual void InitializeComponent() override;

	/** 某物品最多可往背包加入的数量 */
	UFUNCTION(BlueprintCallable)
	int32 GetMaxAddNum(const FName& RowName);

	/** 往背包放入物品 */
	UFUNCTION(BlueprintCallable)
	int32 AddItem(const FName& RowName, int32 Count);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(const FName& RowName, int32 Count);
	void RemoveItem(int32 Index, int32 Count);

	UFUNCTION(BlueprintCallable)
	int32 CountItem(const FName& RowName) const;
	FBackpackItemInfo FindItem(int32 Index) const;
	int32 FindItem(const FName& RowName) const;

	UFUNCTION(BlueprintCallable)
	int32 GetCurGrid() const;

	UFUNCTION(BlueprintCallable)
	float GetCurMass() const;

	void Reorder();

	const TArray<FBackpackItemInfo>& GetItemList() const { return ItemList; }

public:
	/** 背包物品改变时发出的广播 */
	UPROPERTY(BlueprintAssignable, Category = "Backpack")
	FOnBackpackChangedEvent OnChanged;

public:
	/** 背包格子数，不能小于0 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxGrid = 8;

	/** 背包承重（容量），单位kg；-1代表无限承重 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxBearing = 100.f;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FBackpackItemInfo> ItemList;
};
