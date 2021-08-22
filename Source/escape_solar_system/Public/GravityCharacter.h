// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MassActorInterface.h"
#include "Controllable.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GravityCharacter.generated.h"

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AGravityCharacter : public ACharacter, public IMassActorInterface, public IControllable
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(Category = GravityCharacter, BlueprintCallable)
	void SetVelocity(const FVector& Velocity);

	void SetNearSpaceship(APawn* Spaceship);

	virtual void Controlled() override;
	virtual void UnControlled() override;
	virtual FVector GetVelocity() const override;
	virtual void GetHP(float& Current, float& Max) const override;
	virtual void GetMP(float& Current, float& Max) const override;
	virtual float GetGravityAccel() const override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;

	void Drive();
	void Turn(float Value);
	void LookUp(float Value);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveUp(float Value);

private:
	/** 最大功率(推进力)，单位N（1N=1 kg·m/s²）；角色暂且固定50N */
	const float Power = 50;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera = nullptr;

	class UGravityMovementComponent* Movement = nullptr;
	APawn* NearSpaceship = nullptr;
	APawn* CurrentVehicle = nullptr;
};
