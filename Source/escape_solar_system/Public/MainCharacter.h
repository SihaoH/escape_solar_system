// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

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
	static AMainCharacter* GetInstance() { return Instance; }

public:
	UFUNCTION(BlueprintCallable)
	void SetVelocity(const FVector& Velocity);
	void ResetProperties();

	class UBackpackComponent* GetBackpack() const { return Backpack; }
	class ASpaceship* FindSpaceship() const;
	class AEarthBaseActor* FindEarthBase() const;
	class UBodyComponent* GetBodyComponent() const { return Body; }
	class UEngineComponent* GetEngineComponent() const { return Engine; }
	FORCEINLINE float GetMass() const;

	virtual FVector GetVelocity() const override;
	virtual void GetHP(float& Current, float& Maximum) const override;
	virtual void GetMP(float& Current, float& Maximum) const override;
	virtual float GetGravityAccel() const override;

protected:
	virtual void Controlled() override;
	virtual void UnControlled() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Thrusting(FVector Force) override;
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;

	void DriveShip();
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

private:
	static AMainCharacter* Instance;

private:
	class UGravityMovementComponent* Movement = nullptr;
	class ASpaceship* CurrentVehicle = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APickableItemActor* PickableItem = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBackpackComponent* Backpack = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBodyComponent* Body = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UEngineComponent* Engine = nullptr;

private:
	friend class UMenuLevel;
	/** 属性等级 */
	int32 LevelStrength = 0; //包含Mass和HP
	int32 LevelBackpack = 0; //背包承重
	int32 LevelShieldCold = 0;
	int32 LevelShieldHeat = 0;
	int32 LevelShieldPress = 0;
	int32 LevelEngine = 0; //包含Power、Mass、EPR、EMR
	int32 LevelEnergy = 0;
};
