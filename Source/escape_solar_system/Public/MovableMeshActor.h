// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MassActorInterface.h"
#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovableMeshActor.generated.h"

/**
 * 可移动（并接受引力）的静态网格体类
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AMovableMeshActor : public AStaticMeshActor, public IMassActorInterface
{
	GENERATED_BODY()

public:
	AMovableMeshActor();
	
protected:
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;
};
