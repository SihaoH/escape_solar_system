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
	virtual FText GetLabelName() const override;
	void ResetProperties();

	FORCEINLINE class AMainCharacter* GetPilot() const { return CurrentPilot; }

	UFUNCTION(BlueprintCallable)
	void Destroy();

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMass() const { return ShipMesh->GetMass(); }

	UFUNCTION(BlueprintPure)
	virtual float GetGravityAccel() const override;

	UFUNCTION(BlueprintPure)
	void GetLocationInfo(FText& Planet, FVector& Loction) const override { IControllable::GetLocationInfo(Planet, Loction); }

	UFUNCTION()
	void OnHpChanged(float Delta);

protected:
	virtual void Serialize(FArchive& Ar) override;
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

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
	class UBackpackComponent* Backpack = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
	class UBodyComponent* Body = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
	class UEngineComponent* Engine = nullptr;

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
	class AMainCharacter* CurrentPilot = nullptr;

private:
	friend class UMenuTechHelper;

	UPROPERTY(SaveGame)
	FTransform SavedTransform;

	/** 属性等级 */
	UPROPERTY(SaveGame)
	int32 LevelStrength    = 0; //包含Mass和HP
	UPROPERTY(SaveGame)
	int32 LevelBackpack    = 0;
	UPROPERTY(SaveGame)
	int32 LevelShieldCold  = 0;
	UPROPERTY(SaveGame)
	int32 LevelShieldHeat  = 0;
	UPROPERTY(SaveGame)
	int32 LevelShieldPress = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, meta = (AllowPrivateAccess = "true"))
	int32 EngineType       = 0;
	UPROPERTY(SaveGame)
	int32 LevelEngine0     = 0; //包含Power、Mass、EPR、EMR
	UPROPERTY(SaveGame)
	int32 LevelEngine1     = -1;
	UPROPERTY(SaveGame)
	int32 LevelEngine2     = -1;
	UPROPERTY(SaveGame)
	int32 LevelEnergy0     = 0;
	UPROPERTY(SaveGame)
	int32 LevelEnergy1     = -1;
	UPROPERTY(SaveGame)
	int32 LevelEnergy2     = -1;
};
