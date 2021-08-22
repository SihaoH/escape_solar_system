// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PlanetActor.generated.h"


UCLASS()
class ESCAPE_SOLAR_SYSTEM_API APlanetActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	APlanetActor(const FObjectInitializer& ObjectInitializer);
	virtual FVector GetVelocity() const override;

	void SetLooked(bool Looked);
	void SetLocked(bool Locked);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	void CallWidgetFunc(const FName& FuncName, void* FuncParm);
	void PerformRotation(float DeltaTime);
	void PerformRevolution(float DeltaTime);
	void PerformGravity(float DeltaTime);
	void UpdateInfoWidget();

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	/** 星球名称 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	FText Name;

	/** 自转的角速度，以角度为单位 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	float RotationSpeed = 0.0f;

	/** 公转的角速度，以角度为单位 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	float RevolutionSpeed = 0.0f;

	/** 星球的表面重力加速度，单位cm/s² */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	float SurfaceGravity = 1.0f;

	/** 旋转中心的星球 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	AActor* CentralActor = nullptr;

private:
	FTimerHandle InfoTimer;
	float PreDistance = 0.f;
	float RelativeSpeed = 0.f;

	/** 和旋转中心的距离 */
	UPROPERTY(Category = "Planet", VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DistanceRadius = 0.0f;

	/** 星球的半径，会决定对物体的引力 */
	UPROPERTY(Category = "Planet", VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SelfRadius = 1.0f;

	UPROPERTY(Category = "Planet", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* GravityZone = nullptr;

	UPROPERTY(Category = "Planet", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* InfoWidget = nullptr;
};
