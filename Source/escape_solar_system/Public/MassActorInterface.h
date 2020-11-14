﻿// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MassActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMassActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ESCAPE_SOLAR_SYSTEM_API IMassActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Category = "Gravity", BlueprintNativeEvent)
	void GravityActed(FVector Direction, float Accel);
};
