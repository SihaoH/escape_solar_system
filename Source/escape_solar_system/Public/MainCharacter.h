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
	UFUNCTION(Category = MainCharacter, BlueprintCallable)
	static AMainCharacter* GetInstance() { return Instance; }

public:
	UFUNCTION(Category = MainCharacter, BlueprintCallable)
	void SetVelocity(const FVector& Velocity);

	class UBackpackComponent* GetBackpack() const { return Backpack; }
	class ASpaceship* FindSpaceship() const;
	class AEarthBaseActor* FindEarthBase() const;

	virtual FVector GetVelocity() const override;
	virtual void GetHP(float& Current, float& Max) const override;
	virtual void GetMP(float& Current, float& Max) const override;
	virtual float GetGravityAccel() const override;

protected:
	virtual void Controlled() override;
	virtual void UnControlled() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;

	void DriveShip();
	void PickupItem();
	void Turn(float Value);
	void LookUp(float Value);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveUp(float Value);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	static AMainCharacter* Instance;

private:
	/** 最大功率(推进力)，单位N（1N=1 kg·m/s²）；角色暂且固定50N */
	const float Power = 50;
	class UGravityMovementComponent* Movement = nullptr;
	class ASpaceship* CurrentVehicle = nullptr;

	UPROPERTY(Category = MainCharacter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APickableItemActor* PickableItem = nullptr;

	UPROPERTY(Category = MainCharacter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera = nullptr;

	UPROPERTY(Category = MainCharacter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBackpackComponent* Backpack = nullptr;
};
