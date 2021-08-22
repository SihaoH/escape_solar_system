// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MassActorInterface.h"
#include "Controllable.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Spaceship.generated.h"

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API ASpaceship : public APawn, public IMassActorInterface, public IControllable
{
	GENERATED_BODY()

public:
	ASpaceship(const FObjectInitializer& ObjectInitializer);

	void SetPilot(APawn* Pilot);

	virtual void GetHP(float& Current, float& Max) const override;
	virtual void GetMP(float& Current, float& Max) const override;
	virtual float GetGravityAccel() const override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Controlled() override;
	virtual void UnControlled() override;
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;

	void Turn(float Value);
	void LookUp(float Value);
	void FreeLook();
	void UnFreeLook();
	void UnDrive();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveUp(float Value);

	void PerformTurn(float DeltaTime);
	void PerformThrust(float DeltaTime);
	void PerformAdjust(float DeltaTime);
	void PerformFOV(float DeltaTime);

private:
	UFUNCTION()
	void OnSomethingClosed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSomethingLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMesh = nullptr;

	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ContactTrigger = nullptr;

	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* OriginComponent = nullptr;

	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera = nullptr;

protected:
	/** 最大功率(推进力)，单位N（1N=1 kg·m/s²） */
	UPROPERTY(Category = Spaceship, EditAnywhere, BlueprintReadWrite)
	float Power = 0;

	/** 当前能量值 */
	UPROPERTY(Category = Spaceship, EditAnywhere, BlueprintReadWrite)
	float MaxEnergy = 1;

	/** 最大能量值 */
	UPROPERTY(Category = Spaceship, EditAnywhere, BlueprintReadWrite)
	float CurEnergy = 0;

	/** 推进速度 */
	UPROPERTY(Category = Spaceship, EditAnywhere, BlueprintReadWrite)
	float ThrustingSpeed = 500;

	/** 重力加速度，单位cm/s² */
	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly)
	float GravityAccel = 0;

	/** 重力方向 */
	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly)
	FVector GravityDirection = FVector::ZeroVector;

private:
	bool bFreeLook = true;

	float ForwardValue = 0;
	float UpValue = 0;

	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ForwardForce = 0;
	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float UpForce = 0;

	APawn* CurrentPilot = nullptr;
};
