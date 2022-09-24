// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CelestialBody.h"
#include "Sun.generated.h"

/**
 * 太阳，继承于天体类，引力作用不太一样
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API ASun : public ACelestialBody
{
	GENERATED_BODY()

public:
	ASun();

private:
	virtual void PerformRotation(float DeltaTime) override;
	virtual void PerformGravity(float DeltaTime) override;

	virtual void OnGravityZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnGravityZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UFluidZoneComponent* InnerZone = nullptr;
};
