// Copyright 2020 H₂S. All Rights Reserved.

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

public:
	virtual FVector GetVelocity() const override;

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

	/** 旋转中心的星球 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	AActor* CentralActor = nullptr;

private:
	void PerformRotation(float DeltaTime);
	void PerformRevolution(float DeltaTime);
	void PerformGravity(float DeltaTime);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(Category = "Planet", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* GravityZone = nullptr;

	/** 和旋转中心的距离 */
	UPROPERTY(Category = "Planet", VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DistanceRadius = 0.0f;
};
