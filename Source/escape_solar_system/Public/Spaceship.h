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
	virtual FText GetLabelName() const override { return Name; }
	void ResetProperties();

	UFUNCTION(BlueprintPure)
	FORCEINLINE class AMainCharacter* GetPilot() const { return CurrentPilot; }

	UFUNCTION(BlueprintCallable)
	void Destroy();

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMass() const { return ShipMesh->GetMass(); }

	UFUNCTION(BlueprintPure)
	virtual float GetGravityAccel() const override;

	UFUNCTION(BlueprintPure)
	void GetLocationInfo(FText& Site, FVector& Loction) const override { IControllable::GetLocationInfo(Site, Loction); }

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
	virtual class UCameraComponent* GetCameraComponent() override { return FollowCamera; }
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;
	virtual void GravityActedGlobally_Implementation(FVector Direction, float Accel) override;
	virtual void BuoyancyActed_Implementation(FVector Force) override;
	virtual void DampingChanged_Implementation(float Linear, float Angular) override;

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
	/** 飞船名称，因为需要[目标指示器]，所添加这两个属性和天体保持一致 */
	UPROPERTY(Category = "飞船", EditAnywhere, BlueprintReadWrite)
	FText Name;
	UPROPERTY(Category = "飞船", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(Category = "飞船", VisibleAnywhere, BlueprintReadOnly, SaveGame)
	class UBackpackComponent* Backpack = nullptr;

	UPROPERTY(Category = "飞船", VisibleAnywhere, BlueprintReadOnly, SaveGame)
	class UBodyComponent* Body = nullptr;

	UPROPERTY(Category = "飞船", VisibleAnywhere, BlueprintReadOnly, SaveGame)
	class UEngineComponent* Engine = nullptr;

private:
	UPROPERTY(Category = "飞船", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMesh = nullptr;

	UPROPERTY(Category = "飞船", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ContactTrigger = nullptr;

	UPROPERTY(Category = "飞船", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* OriginComponent = nullptr;

	UPROPERTY(Category = "飞船", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(Category = "飞船", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera = nullptr;

	UPROPERTY(Category = "飞船", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<class UStaticMeshComponent*> Brackets;

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
