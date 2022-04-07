﻿// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MainFunctionLibrary.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/ObjectMacros.h"
#include "BackpackComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackpackChangedEvent);

/**
 * 背包组件
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UBackpackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBackpackComponent();

	void SetBackpack(EPawnType Type, int32 Level);
	void AddItem(const FName& RowName, int32 Count);
	void RemoveItem(const FName& RowName, int32 Count);
	const TMultiMap<FName, int32>& GetItemList() const { return ItemList; }

	/** 某物品最多可往背包加入的数量 */
	UFUNCTION(BlueprintCallable)
	int32 GetMaxAddNum(const FName& RowName);

	UFUNCTION(BlueprintCallable)
	int32 CountItem(const FName& RowName) const;

	UFUNCTION(BlueprintCallable)
	int32 GetCurCount() const;

	FORCEINLINE float GetMaxLoad() const { return MaxLoad; }
	FORCEINLINE float GetMass() const { return Mass; }

protected:
	void UpdateMass();

public:
	/** 背包物品改变时发出的广播 */
	UPROPERTY(BlueprintAssignable)
	FOnBackpackChangedEvent OnChanged;

private:
	/** 背包承重（容量），单位kg；-1代表无限承重 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxLoad = -1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Mass = 0.f;

	TMultiMap<FName, int32> ItemList;
};
