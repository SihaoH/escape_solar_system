// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "TechData.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExplorePointsChangedSignature, int32, Delta);

/**
 * 玩家状态，记录了探索点数,躯体和飞船的最高等级
 * 在默认游戏模式MainGameMode中指定
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMainPlayerState();

	UFUNCTION(BlueprintPure)
	static AMainPlayerState* Instance() { return s_Instance; }

	void UpdateBestLevel(const TArray<ETech>& Props, int32 Val);
	int32 GetBestLevel(const TArray<ETech>& Props);

	int32 GetDeathCount() const { return DeathCount; }
	void IncreaseDeathCount() { DeathCount += 1; }

	float GetTotalTime() const { return TotalTime; }
	void UpdateTotalTime(float Delta) { TotalTime += Delta; }

	void ChangeExplorePoints(int32 Delta);
	UFUNCTION(BlueprintPure)
	int32 GetExplorePoints();

public:
	UPROPERTY(BlueprintAssignable)
	FExplorePointsChangedSignature ExplorePointsChangedDelegate;

private:
	FORCEINLINE int32* GetBest(const TArray<ETech>& Props);

private:
	static AMainPlayerState* s_Instance;

	UPROPERTY(SaveGame)
	int32 ExplorePoints = 999;
	UPROPERTY(SaveGame)
	int32 DeathCount = 0;
	UPROPERTY(SaveGame)
	float TotalTime = 0;

	UPROPERTY(SaveGame)
	int32 Best_CharBody = 0;
	UPROPERTY(SaveGame)
	int32 Best_CharBackpack = 0;
	UPROPERTY(SaveGame)
	int32 Best_CharShieldCold = 0;
	UPROPERTY(SaveGame)
	int32 Best_CharShieldHeat = 0;
	UPROPERTY(SaveGame)
	int32 Best_CharShieldPress = 0;
	UPROPERTY(SaveGame)
	int32 Best_CharEngine = 0;
	UPROPERTY(SaveGame)
	int32 Best_CharEnergy = 0;

	UPROPERTY(SaveGame)
	int32 Best_ShipBody = 0;
	UPROPERTY(SaveGame)
	int32 Best_ShipBackpack = 0;
	UPROPERTY(SaveGame)
	int32 Best_ShipShieldCold = 0;
	UPROPERTY(SaveGame)
	int32 Best_ShipShieldHeat = 0;
	UPROPERTY(SaveGame)
	int32 Best_ShipShieldPress = 0;
	UPROPERTY(SaveGame)
	int32 Best_ShipEngine[3] = {0, -1, -1};
	UPROPERTY(SaveGame)
	int32 Best_ShipEnergy[3] = {0, -1, -1};
};
