// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MassActorInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Spaceship.generated.h"

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API ASpaceship : public APawn, public IMassActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpaceship();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;
	void Turn(float Value);
	void LookUp(float Value);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Raise(float Value);
	void TakeOff(float Value);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMesh = nullptr;

	//UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class USceneComponent* ViewOrigin = nullptr;

	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(Category = Spaceship, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera = nullptr;

protected:
	UPROPERTY(Category = Spaceship, EditAnywhere, BlueprintReadWrite)
	float Power = 0;

	UPROPERTY(Category = Spaceship, EditAnywhere, BlueprintReadWrite)
	float MaxEnergy = 0;

	UPROPERTY(Category = Spaceship, EditAnywhere, BlueprintReadWrite)
	float CurEnergy = 0;

private:
	float ForwardValue = 0;
	float UpValue = 0;
	float RightValue = 0;
	float RaiseValue = 0;

	float ForwardForce = 0;
	float UpForce = 0;
	float RightForce = 0;
	float RaiseForce = 0;
};
