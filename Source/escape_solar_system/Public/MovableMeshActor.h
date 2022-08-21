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
	virtual void BeginPlay() override;

	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;
	virtual void BuoyancyActed_Implementation(FVector Force) override;
	virtual void DampingChanged_Implementation(float Linear, float Angular) override;

private:
	/** 浮力 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SelfBuoyancy = 1.0;
};
