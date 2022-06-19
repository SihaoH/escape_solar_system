// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "LevelData.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExplorePointsChangedSignature, int32, Delta);

/**
 * 玩家状态，记录了探索点数,躯体和飞船的最高等级
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMainPlayerState();

	UFUNCTION(BlueprintPure)
	static AMainPlayerState* Instance() { return s_Instance; }

	void UpdateBestLevel(const TArray<ELevel>& Props, int32 Val);
	int32 GetBestLevel(const TArray<ELevel>& Props);

	void ChangeExplorePoints(int32 Delta);
	UFUNCTION(BlueprintPure)
	int32 GetExplorePoints();

public:
	UPROPERTY(BlueprintAssignable)
	FExplorePointsChangedSignature ExplorePointsChangedDelegate;

private:
	FORCEINLINE int32* GetBest(const TArray<ELevel>& Props);

private:
	static AMainPlayerState* s_Instance;

	int32 ExplorePoints = 999;

	int32 Best_CharBody = 0;
	int32 Best_CharBackpack = 0;
	int32 Best_CharShieldCold = 0;
	int32 Best_CharShieldHeat = 0;
	int32 Best_CharShieldPress = 0;
	int32 Best_CharEngine = 0;
	int32 Best_CharEnergy = 0;

	int32 Best_ShipBody = 0;
	int32 Best_ShipBackpack = 0;
	int32 Best_ShipShieldCold = 0;
	int32 Best_ShipShieldHeat = 0;
	int32 Best_ShipShieldPress = 0;
	int32 Best_ShipEngine[3] = {0, -1, -1};
	int32 Best_ShipEnergy[3] = {0, -1, -1};
};
