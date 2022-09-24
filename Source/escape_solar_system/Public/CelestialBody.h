// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "CelestialBody.generated.h"

UENUM(BlueprintType)
enum class ECelestialBodyType : uint8
{
	Star,
	Planet,
	Moon,
	Comet
};

/**
 * 天体类，发射引力、信息显示等
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API ACelestialBody : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ACelestialBody();
	virtual FVector GetVelocity() const override;

	void SetLooked(bool Looked);
	void SetLocked(bool Locked);
	float GetSurfaceGravity() const { return SurfaceGravity; }
	void CalcGravityResult(AActor* Target, FVector& Direction, float& Accel) const;
	FORCEINLINE FText GetLabelName() const { return Name; }
	FORCEINLINE ECelestialBodyType GetCelestialType() const { return Type; }

protected:
	virtual void Serialize(FArchive& Ar) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void PerformRotation(float DeltaTime);
	virtual void PerformGravity(float DeltaTime);
	void UpdateInfoWidget();

private:
	UFUNCTION()
	virtual void OnGravityZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnGravityZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	/** 星球类型 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	ECelestialBodyType Type = ECelestialBodyType::Planet;

	/** 星球名称 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	FText Name;

	/** 星球图标 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon;

	/** 自转的角速度，以角度为单位，单位°/s */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	float RotationSpeed = 0.0f;

	/** 星球的表面重力加速度，单位cm/s² */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	float SurfaceGravity = 1.0f;

	/** 星球的半径，单位cm，会决定对物体的引力；如不指定，则自动获取网格体的大小，可能会以最高峰为准 */
	UPROPERTY(Category = "Planet", EditAnywhere, BlueprintReadWrite)
	float SelfRadius = -1.f;

private:
	FTimerHandle InfoTimer;
	float PreDistance = 0.f;
	float RelativeSpeed = 0.f;

	UPROPERTY(SaveGame)
	FTransform SavedTransform;

	/** 和旋转中心的距离 */
	UPROPERTY(Category = "Planet", VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DistanceRadius = 0.0f;

	UPROPERTY(Category = "Planet", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* GravityZone = nullptr;

	UPROPERTY(Category = "Planet", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* InfoWidget = nullptr;
	class UPlanetInfo* InfoWidgetObject = nullptr;
};
