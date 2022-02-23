// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

/**
 * 玩家状态，记录了探索点数、躯体和飞船的等级
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMainPlayerState();

public:
	int32 ExplorePoint = 0;

	int32 Best_CharBackpackLoad = 0;
	int32 Best_CharBodyLevel = 0;
	int32 Best_CharBodyShieldCold = 0;
	int32 Best_CharBodyShieldHeat = 0;
	int32 Best_CharBodyShieldPress = 0;
	int32 Best_CharEngineLevel = 0;
	int32 Best_CharEngineEnergy = 0;

	int32 Best_ShipBodyLevel = 0;
	int32 Best_ShipBodyLoad = 0;
	int32 Best_ShipBodyShieldCold = 0;
	int32 Best_ShipBodyShieldHeat = 0;
	int32 Best_ShipBodyShieldPress = 0;
	int32 Best_ShipEngineType = 0;
	int32 Best_ShipEngineLevel[3];
	int32 Best_ShipEngineEnergy[3];
};
