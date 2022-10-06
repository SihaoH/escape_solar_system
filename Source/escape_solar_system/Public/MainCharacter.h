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
	void SetVelocity(const FVector& Velocity);
	void ResetProperties();

	UFUNCTION(BlueprintCallable)
	void Destroy();

	UFUNCTION(BlueprintPure)
	bool IsDriving() const { return !!CurrentVehicle; }

	UFUNCTION(BlueprintPure)
	class ASpaceship* FindSpaceship() const;

	UFUNCTION(BlueprintPure)
	class AEarthBase* FindEarthBase() const;

	UFUNCTION(BlueprintPure)
	class ANPC* GetTalkableNPC() const { return TalkableNPC; }
	void SetTalkableNPC(class ANPC* NPC) { TalkableNPC = NPC; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMass() const;

	virtual FVector GetVelocity() const override;

	UFUNCTION(BlueprintPure)  // unreal.js不支持多重继承，所以IControllable父类的方法要再次声明UFUNCTION
	virtual FText GetLabelName() const override;

	UFUNCTION(BlueprintPure)
	virtual float GetGravityAccel() const override;

	UFUNCTION(BlueprintPure)
	void GetLocationInfo(FText& Site, FVector& Loction) const override { IControllable::GetLocationInfo(Site, Loction); }

protected:
	virtual void Serialize(FArchive& Ar) override;
	virtual void Controlled() override;
	virtual void UnControlled() override;
	virtual class UCameraComponent* GetCameraComponent() override { return FollowCamera; }
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Thrusting(FVector Force) override;
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;
	virtual void GravityActedGlobally_Implementation(FVector Direction, float Accel) override;
	virtual void BuoyancyActed_Implementation(FVector Force) override;
	virtual void DampingChanged_Implementation(float Linear, float Angular) override;

	UFUNCTION()
	void DriveShip();

	UFUNCTION()
	void PickupItem();

	UFUNCTION()
	void TalkToNPC();

	void Turn(float Value);
	void LookUp(float Value);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveUp(float Value);

	void UpdateMass();
	void CheckPickup();
	void CheckNPC();

	UFUNCTION()
	void OnHpChanged(float Delta);
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
	static class ACameraActor* DeathCamera;
	class UGravityMovementComponent* Movement = nullptr;
	class ASpaceship* CurrentVehicle = nullptr;
	class ANPC* TalkableNPC = nullptr;
	class UPickableItem* PickableItem = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera = nullptr;

private:
	friend class UMenuTechHelper;

	float SunGravityAccel = 0.f;

	UPROPERTY(SaveGame)
	FTransform SavedTransform;

	/** 属性等级 */
	UPROPERTY(SaveGame)
	int32 LevelStrength    = 0; //包含Mass和HP
	UPROPERTY(SaveGame)
	int32 LevelBackpack    = 0; //背包承重
	UPROPERTY(SaveGame)
	int32 LevelShieldCold  = 0;
	UPROPERTY(SaveGame)
	int32 LevelShieldHeat  = 0;
	UPROPERTY(SaveGame)
	int32 LevelShieldPress = 0;
	UPROPERTY(SaveGame)
	int32 LevelEngine      = 0; //包含Power、Mass、EPR、EMR
	UPROPERTY(SaveGame)
	int32 LevelEnergy      = 0;
};
