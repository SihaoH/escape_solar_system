﻿// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PlanetActor.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API APlanetActor : public AStaticMeshActor
{
	GENERATED_BODY()
	APlanetActor(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	/** 自转的角速度，以角度为单位 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	float RotationSpeed = 0.0f;

	/** 公转的角速度，以角度为单位 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	float RevolutionSpeed = 0.0f;

	/** 行星的质量，会决定对物体的引力，不能为0 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	float Mass = 1.0f;

	/** 旋转中心的坐标 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	FVector CentralLocation = FVector::ZeroVector;

	/** 和旋转中心的距离 */
	UPROPERTY(Category = "Planet", VisibleInstanceOnly, BlueprintReadOnly)
	float DistanceRadius = 0.0f;

private:
	void PerformRotation(float DeltaTime);
	void PerformRevolution(float DeltaTime);
	void PerformGravity(float DeltaTime);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(Category = "Planet", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* GravityZone = nullptr;
};
