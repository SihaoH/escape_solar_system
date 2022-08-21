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

	FORCEINLINE class APlanetActor* GetPlanetOwner() { return PlanetOwner; }
	FORCEINLINE class UFluidZoneComponent* GetFluidZone() { return FluidZoneStack.Num() > 0 ? FluidZoneStack.Last() : nullptr; }

protected:
	/** 水浮力，跟重力的比率；因为接口不能声明UPROPERTY，所以该值要在子类中赋值 */
	float Buoyancy = 1.0f;

private:
	class APlanetActor* PlanetOwner = nullptr;
	TArray<class UFluidZoneComponent*> FluidZoneStack;

	friend class APlanetActor;
	friend class UFluidZoneComponent;
};
