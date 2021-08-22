// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Controllable.generated.h"


UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
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
	virtual void GetHP(float& Current, float& Max) const;

	UFUNCTION(BlueprintCallable, Category = Controllable)
	virtual void GetMP(float& Current, float& Max) const;

	UFUNCTION(BlueprintCallable, Category = Controllable)
	virtual float GetGravityAccel() const;

	/** 获取可显示的坐标值（单位m） */
	UFUNCTION(BlueprintCallable, Category = Controllable)
	virtual void GetLocation(int& X, int& Y, int& Z);

	virtual void Controlled() {};
	virtual void UnControlled() {};

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
