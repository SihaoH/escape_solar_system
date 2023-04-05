// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MainLibrary.h"
#include "MainSaveGame.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EngineComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnergyChangedSignature, float, Delta);

/**
 * 发动机组件，推动机体运动
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UEngineComponent : public UActorComponent, public ISaveGameClass
{
	GENERATED_BODY()

public:

	UPROPERTY(SaveGame)
	float TestValue = 0.f;
	UEngineComponent();

	void SetEngine(EPawnType PType, int32 Level, int32 Type = 0);
	void SetEnergy(EPawnType PType, int32 Level, int32 Type = 0);

	void ChangeEnergy(float Delta);

	void MoveForward(float Value);
	void MoveUp(float Value);

	FORCEINLINE float GetEngineMass() const { return Mass; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetEnergyMass() const { return CurrentEnergy * EMRatio; }
	FORCEINLINE float GetTotalMass() const { return GetEngineMass() + GetEnergyMass(); }
	FORCEINLINE float GetPower() const { return Power; }
	FORCEINLINE float GetEPRatio() const { return EPRatio; }
	FORCEINLINE float GetEMRatio() const { return EMRatio; }
	FORCEINLINE float GetCurrentEnergy() const { return CurrentEnergy; }
	FORCEINLINE float GetMaximumEnergy() const { return MaximumEnergy; }
	FORCEINLINE float GetForwardForce() const { return ForwardForce; }
	FORCEINLINE float GetUpForce() const { return UpForce; }

private:
	void UseEnergy(float Used);

public:
	UPROPERTY(BlueprintAssignable)
	FEnergyChangedSignature EnergyChangedDelegate;

private:
	/** 引擎本身的质量kg */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Mass = 1.f;

	/** 最大功率(推进力)，单位N（1N=1 kg·m/s²），引擎使用cm为单位，所以调用时要乘以100 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Power = 0;

	/** 能耗比，即1单位能量所能产生的推力N */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float EPRatio = 1.f;

	/** 能质比，即1单位能量所带来的质量kg */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float EMRatio = 1.f;

	/** 当前能量值 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, meta = (AllowPrivateAccess = "true"))
	float CurrentEnergy = 0;

	/** 最大能量值 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaximumEnergy = 1;

	/** 当前向前的推力，若为负则向后 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ForwardForce = 0;
	/** 当前向上的推力，若为负则向下 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float UpForce = 0;
};
