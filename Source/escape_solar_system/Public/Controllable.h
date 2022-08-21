// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
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
	UFUNCTION(BlueprintCallable, Category = Controllable)
	virtual FText GetDisplayName() const = 0;

	UFUNCTION(BlueprintCallable, Category = Controllable)
	virtual float GetGravityAccel() const;

	/** 获取可显示的坐标值（单位m） */
	UFUNCTION(BlueprintCallable, Category = Controllable)
	virtual void GetLocation(int& X, int& Y, int& Z);

	template <typename TargetClass>
	FORCEINLINE TargetClass* FindByLineTrace(float Distance);

	virtual void Controlled() {};
	virtual void UnControlled() {};
	virtual void Thrusting(FVector Force) {};

	void ChangePawn(class APawn* NewPawn);
	void LookPlanet();
	void LockPlanet();

	static void ClearUp();

private:
	/** 被注视的星球 */
	static class APlanetActor* LookedPlanet;
	/** 被锁定的星球 */
	static class APlanetActor* LockedPlanet;
};

template<typename TargetClass>
FORCEINLINE TargetClass* IControllable::FindByLineTrace(float Distance)
{
	AActor* Self = Cast<AActor>(this);
	const APlayerController* Controller = UGameplayStatics::GetPlayerController(Self->GetWorld(), 0);
	const APlayerCameraManager* CameraManager = Controller->PlayerCameraManager;
	const FVector Start = CameraManager->GetCameraLocation();
	const FVector End = Start + CameraManager->GetTransformComponent()->GetForwardVector() * Distance; //最长到64km
	FHitResult OutHit;
	TargetClass* HitTarget = nullptr;
	if (UKismetSystemLibrary::LineTraceSingle(
		Self->GetWorld(), Start, End,
		ETraceTypeQuery::TraceTypeQuery1, true, { Self }, EDrawDebugTrace::ForOneFrame, OutHit, true))
	{
		HitTarget = Cast<TargetClass>(OutHit.GetActor());
	}
	return HitTarget;
}
