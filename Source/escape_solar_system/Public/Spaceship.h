// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MassActorInterface.h"
#include "Controllable.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Spaceship.generated.h"

/**
 * 主载具（飞船）类
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API ASpaceship : public APawn, public IMassActorInterface, public IControllable
{
	GENERATED_BODY()

public:
	ASpaceship();

	void SetPilot(class AMainCharacter* Pilot);
	void InitState();
	class UBackpackComponent* GetBackpack() const { return Storehouse; }
	class UBodyComponent* GetBodyComponent() const { return Body; }
	class UEngineComponent* GetEngineComponent() const { return Engine; }
	FORCEINLINE float GetMass() const { return ShipMesh->GetMass(); }

	virtual void GetHP(float& Current, float& Max) const override;
	virtual void GetMP(float& Current, float& Max) const override;
	virtual float GetGravityAccel() const override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Controlled() override;
	virtual void UnControlled() override;
	virtual void Thrusting(FVector Force) override;
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
	void PerformAdjust(float DeltaTime);
	void PerformFOV(float DeltaTime);
	void UpdateMass();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ContactTrigger = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* OriginComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera = nullptr;

protected:
	/** 重力加速度，单位cm/s² */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GravityAccel = 0;

	/** 重力方向 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector GravityDirection = FVector::ZeroVector;

private:
	bool bFreeLook = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AMainCharacter* CurrentPilot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBackpackComponent* Storehouse = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBodyComponent* Body = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UEngineComponent* Engine = nullptr;

private:
	/** 属性等级 */
	int32 BodyLevel = 0; //包含Mass和HP
	int32 BackpackLoad = 0; //背包承重
	int32 BodyShieldCold = 0;
	int32 BodyShieldHeat = 0;
	int32 BodyShieldPress = 0;
	int32 EngineType = 0;
	int32 EngineLevel = 0; //包含Power、Mass、EPR、EMR
	int32 EngineEnergy = 0;
};
