// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BodyComponent.generated.h"

/**
 * 机体组件，包含强度（血量）、护盾、质量等信息
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UBodyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBodyComponent();

	void SetupCollisionComponent(UPrimitiveComponent* PrimitiveComponent);
	void ChangeHP(int32 Delta);
	void GetHP(int32& CurHP, int32& MaxHP);

protected:
	virtual void BeginPlay() override;

private:
	void CheckEnvironment();

	UFUNCTION()
	void OnComponentHitted(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	FTimerHandle EnvTimer;
	UPrimitiveComponent* CollisionComponent = nullptr;

	/** 机体质量，强度(HP)会影响质量 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Mass = 1.f;

	/** 当前血量 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentHP = 0;

	/** 最大血量，即机体强度 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MaximumHP = 1;

	/** 耐冷护盾 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ShieldCold = 0;

	/** 耐热护盾 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ShieldHeat = 0;

	/** 耐压护盾 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ShieldPress = 0;

	/** 机体/躯体当前的温度，当没受到外界温度影响时（比如真空中），物体温度理应维持稳定 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentTemp = 0;

	/** 当前气压，真空时为0 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentPress = 0;
};
