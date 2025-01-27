﻿// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <UObject/Interface.h>
#include <Camera/CameraComponent.h>
#include "Controllable.generated.h"

UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UControllable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 可控制的（角色）接口类，提供获取HP或MP等接口
 */
class ESCAPE_SOLAR_SYSTEM_API IControllable
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual FText GetLabelName() const = 0;
	virtual float GetGravityAccel() const;
	virtual void GetLocationInfo(FText& Site, FVector& Loction) const;

	template <typename TargetClass>
	FORCEINLINE TargetClass* FindByLineTrace(float Distance);

	virtual void Controlled() {};
	virtual void UnControlled() {};
	virtual void Thrusting(FVector Force) {};
	virtual class UCameraComponent* GetCameraComponent() = 0;

	void ChangePawn(class APawn* NewPawn);
	void CheckCelestialBody();
	void LockCelestialBody();

	static void ClearUp();

private:
	/** 被注视的星球 */
	static class ACelestialBody* LookedCelestialBody;
	/** 被锁定的星球 */
	static class ACelestialBody* LockedCelestialBody;
};

template<typename TargetClass>
FORCEINLINE TargetClass* IControllable::FindByLineTrace(float Distance)
{
	AActor* Self = Cast<AActor>(this);
	const UCameraComponent* Camera = GetCameraComponent();
	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * Distance;
	FHitResult OutHit;
	TargetClass* HitTarget = nullptr;
	if (UKismetSystemLibrary::LineTraceSingle(
		Self->GetWorld(), Start, End,
		ETraceTypeQuery::TraceTypeQuery1, true, { Self }, EDrawDebugTrace::ForOneFrame, OutHit, true))
	{
		if (TargetClass::StaticClass()->IsChildOf(UPrimitiveComponent::StaticClass()))
		{
			HitTarget = Cast<TargetClass>(OutHit.GetComponent());
		}
		else
		{
			HitTarget = Cast<TargetClass>(OutHit.GetActor());
		}
	}
	return HitTarget;
}
