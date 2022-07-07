// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MainLibrary.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BodyComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHpChangedSignature, float, Delta);

/**
 * 机体组件，包含强度（血量）、护盾、质量等信息
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UBodyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBodyComponent();

	UFUNCTION()
	void OnComponentHitted(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SetStrength(EPawnType Type, int32 Level);
	void SetShieldCold(EPawnType Type, int32 Level);
	void SetShieldHeat(EPawnType Type, int32 Level);
	void SetShieldPress(EPawnType Type, int32 Level);

	UFUNCTION(BlueprintCallable)
	void ChangeHP(int32 Delta);
	FORCEINLINE float GetMass() const { return Mass; }
	FORCEINLINE float GetCurrentHP() const { return CurrentHP; }
	FORCEINLINE float GetMaximumHP() const { return MaximumHP; }
	FORCEINLINE float GetShieldCold() const { return ShieldCold; }
	FORCEINLINE float GetShieldHeat() const { return ShieldHeat; }
	FORCEINLINE float GetShieldPress() const { return ShieldPress; }
	FORCEINLINE float GetCurrentTemp() const { return CurrentTemp; }
	FORCEINLINE float GetCurrentPress() const { return CurrentPress; }

protected:
	virtual void BeginPlay() override;

private:
	void CheckEnvironment();

public:
	UPROPERTY(BlueprintAssignable)
	FHpChangedSignature HpChangedDelegate;

private:
	FTimerHandle EnvTimer;

	/** 机体质量，强度(HP)会影响质量 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Mass = 1.f;

	/** 当前血量 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentHP = 0;

	/** 最大血量，即机体强度 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaximumHP = 1;

	/** 耐冷护盾 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ShieldCold = 0;

	/** 耐热护盾 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ShieldHeat = 0;

	/** 耐压护盾 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ShieldPress = 0;

	/** 机体/躯体当前的温度，当没受到外界温度影响时（比如真空中），物体温度理应维持稳定 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentTemp = 0;

	/** 当前气压，真空时为0 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentPress = 0;
};
