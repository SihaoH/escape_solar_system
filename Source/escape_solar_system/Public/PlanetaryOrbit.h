// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlanetaryOrbit.generated.h"

/**
 * 星球的轨道体系，星球要执行公转，比如附着到一个轨道体系上
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API APlanetaryOrbit : public AActor
{
	GENERATED_BODY()
public:
	APlanetaryOrbit();

protected:
	virtual void Tick(float DeltaTime) override;

protected:
	/** 公转的角速度，以角度为单位，单位°/s */
	UPROPERTY(Category = Planet, EditAnywhere, BlueprintReadWrite)
	float RevolutionSpeed = 0.0f;

	/** 旋转中心的星球 */
	UPROPERTY(Category = Planet, EditAnywhere, BlueprintReadWrite)
	AActor* CentralActor = nullptr;
};
