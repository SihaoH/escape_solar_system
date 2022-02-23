// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EngineComponent.generated.h"

/**
 * 发动机组件，推动机体运动
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UEngineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEngineComponent();

	void MoveForward(float Value);
	void MoveUp(float Value);

	FORCEINLINE float GetMass() const { return Mass + CurrentEnergy * EMRatio; };

protected:
	virtual void BeginPlay() override;

private:
	void UseEnergy(float Used);

public:
	/** 引擎本身的质量kg */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Mass = 1.f;

	/** 最大功率(推进力)，单位N（1N=1 kg·m/s²），引擎使用cm为单位，所以调用时要乘以100 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Power = 0;

	/** 能耗比，即1单位能量所能产生的推力N */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float EPRatio = 1.f;

	/** 能质比，即1单位能量所带来的质量kg */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float EMRatio = 1.f;

	/** 当前能量值 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentEnergy = 0;

	/** 最大能量值 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaximumEnergy = 1;

	/** 当前向前的推力，若为负则向后 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ForwardForce = 0;
	/** 当前向上的推力，若为负则向下 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float UpForce = 0;
};
