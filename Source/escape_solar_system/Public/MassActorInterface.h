// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MassActorInterface.generated.h"


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

public:
	/** 重力作用，每帧触发 */
	UFUNCTION(BlueprintNativeEvent)
	void GravityActed(FVector Direction, float Accel);

	/** 浮力作用，每帧触发 */
	UFUNCTION(BlueprintNativeEvent)
	void BuoyancyActed(FVector Force);

	/** 浮力作用，每帧触发 */
	UFUNCTION(BlueprintNativeEvent)
	void DampingChanged(float Linear, float Angular);

	// 计算浮力，全部简化成球体来计算
	FORCEINLINE float CalcDensity(float Mass, float Radius) const { return Mass / (4.18879f * FMath::Cube(Radius / 100)); }
	FORCEINLINE class ACelestialBody* GetPlanetOwner() const { return PlanetOwner; }
	FORCEINLINE class UFluidZoneComponent* GetFluidZone() const { return FluidZoneStack.Num() > 0 ? FluidZoneStack.Last() : nullptr; }

protected:
	/** 密度，要在子类中计算并赋值 */
	float Density = 1000.f;

private:
	class ACelestialBody* PlanetOwner = nullptr;
	TArray<class UFluidZoneComponent*> FluidZoneStack;

	friend class ACelestialBody;
	friend class UFluidZoneComponent;
};
