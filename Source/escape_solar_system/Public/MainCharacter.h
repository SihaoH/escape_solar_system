// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "GravityMovementComponent.h"
#include "MainLevelScriptActor.h"
#include "MassActorInterface.h"
#include "Controllable.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

/**
 * 主角类
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AMainCharacter : public ACharacter, public IMassActorInterface, public IControllable
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetVelocity(const FVector& Velocity);
	void ResetProperties();

	UFUNCTION(BlueprintPure)
	bool IsDriving() const { return !!CurrentVehicle; }

	UFUNCTION(BlueprintPure)
	class ASpaceship* FindSpaceship() const;

	UFUNCTION(BlueprintPure)
	class AEarthBase* FindEarthBase() const;

	UFUNCTION(BlueprintPure)
	class APickableItemActor* FindPickableItem() const;

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMass() const { return Movement->Mass; }

	virtual FVector GetVelocity() const override;
	virtual float GetGravityAccel() const override;

protected:
	virtual void Controlled() override;
	virtual void UnControlled() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Thrusting(FVector Force) override;
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;

	UFUNCTION()
	void DriveShip();

	UFUNCTION()
	void PickupItem();
	void Turn(float Value);
	void LookUp(float Value);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveUp(float Value);

	void UpdateMass();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBackpackComponent* Backpack = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBodyComponent* Body = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEngineComponent* Engine = nullptr;

private:
	class UGravityMovementComponent* Movement = nullptr;
	class ASpaceship* CurrentVehicle = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera = nullptr;

private:
	friend class UMenuLevelHelper;
	/** 属性等级 */
	int32 LevelStrength    = 0; //包含Mass和HP
	int32 LevelBackpack    = 0; //背包承重
	int32 LevelShieldCold  = 0;
	int32 LevelShieldHeat  = 0;
	int32 LevelShieldPress = 0;
	int32 LevelEngine      = 0; //包含Power、Mass、EPR、EMR
	int32 LevelEnergy      = 0;
};
