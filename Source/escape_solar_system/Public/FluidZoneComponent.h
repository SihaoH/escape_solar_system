// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include <Components/PostProcessComponent.h>
#include "FluidZoneComponent.generated.h"

UCLASS(ClassGroup = Rendering, collapsecategories, hidecategories = (Object), editinlinenew, meta = (BlueprintSpawnableComponent))
class ESCAPE_SOLAR_SYSTEM_API UMeshPostProcessComponent : public UPostProcessComponent
{
	GENERATED_BODY()

	virtual bool EncompassesPoint(FVector Point, float SphereRadius, float* OutDistanceToPoint) override;
	virtual FPostProcessVolumeProperties GetProperties() const override
	{
		FPostProcessVolumeProperties Ret = UPostProcessComponent::GetProperties();
		Ret.bIsUnbound = bUnbound != 0 || Cast<UStaticMeshComponent>(GetAttachParent()) == nullptr;
		return Ret;
	}

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void Serialize(FArchive& Ar) override;
};

/**
 * 流体（气体和液体）区域，会对有质量的物体产生浮力、阻力；对玩家角色载具产生温度、压力，以及后期处理的视觉效果
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ESCAPE_SOLAR_SYSTEM_API UFluidZoneComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UFluidZoneComponent();

	/** 热传递，通过目标物体的温度，推算出将要变化的温度；由BodyComponent每秒调用 */
	float HeatTransfer(float Current) const;

	/** 压力传递；虽然在现实中，压力/重力/浮力三者是相关的，但这里压力只作为游戏数值设定的一个独立属性，只和BlendDistance相关 */
	float PressTransfer(FVector Location) const;

protected:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	class APlanetActor* AttachedPlanet = nullptr;
	// 上一个流体区域，用于嵌套的情况
	UFluidZoneComponent* PreFluidZone = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMeshPostProcessComponent* PostProcess = nullptr;

	/** 密度，用于计算浮力，标准水的密度为1000 kg/m³ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Density = 1000.f;

	/** 流体对物体产生的线性阻尼 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float LinerDamping = 0.01;

	/** 流体对物体产生的角(旋转)阻尼 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AngularDamping = 0.01;

	/** 温度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Temperature = 25.f;

	/** 热传递效率，即温度每秒的变化百分比 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Efficiency = 0.5f;

	/** 压力 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Pressure = 25.f;

	/** 压力的混合距离比率，即进入到流体内部一定范围后，压力逐渐达到100% */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlendDistance = 0.5f;
};
