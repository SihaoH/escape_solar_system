// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MassActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UMassActorInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 有质量的物品，继承该接口后，当物品进入星体的引力范围，就会收到GravityActed事件
 */
class ESCAPE_SOLAR_SYSTEM_API IMassActorInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Category = Gravity, BlueprintNativeEvent)
	void GravityActed(FVector Direction, float Accel);
};
