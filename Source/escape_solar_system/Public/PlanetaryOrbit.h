// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlanetaryOrbit.generated.h"

/**
 * 由于USceneComponent内部实现旋转时，太小的值会被忽略，所以要继承并重写MoveComponentImpl方法
 */
UCLASS(ClassGroup = Rendering, collapsecategories, hidecategories = (Object), editinlinenew, meta = (BlueprintSpawnableComponent))
class ESCAPE_SOLAR_SYSTEM_API UOrbitSceneComponent : public USceneComponent
{
	GENERATED_BODY()

protected:
	virtual bool MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* Hit = NULL, EMoveComponentFlags MoveFlags = MOVECOMP_NoFlags, ETeleportType Teleport = ETeleportType::None) override;

private:
	bool InternalSetWorldLocationAndRotation(FVector NewLocation, const FQuat& NewQuat, bool bNoPhysics = false, ETeleportType Teleport = ETeleportType::None);
};

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
