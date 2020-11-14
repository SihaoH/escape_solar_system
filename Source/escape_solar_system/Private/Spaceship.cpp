// Copyright 2020 H₂S. All Rights Reserved.


#include "Spaceship.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASpaceship::ASpaceship()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));

	//ViewOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("ViewOrigin"));
	//ViewOrigin->SetupAttachment(ShipMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArm->SetupAttachment(ViewOrigin);
	SpringArm->SetRelativeLocation(FVector(0, 0, 250));
	SpringArm->TargetArmLength = 1000;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ASpaceship::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpaceship::GravityActed_Implementation(FVector Direction, float Accel)
{
	ShipMesh->AddForce(Direction * Accel, NAME_None, true);
	GEngine->AddOnScreenDebugMessage(4, 1, FColor::Green, TEXT("Gravity:") + FString::SanitizeFloat(Accel));
}

void ASpaceship::Turn(float Value)
{
	SpringArm->AddRelativeRotation(FRotator(0, Value, 0));
}

void ASpaceship::LookUp(float Value)
{
	Value = Value * -1;
	FRotator current = SpringArm->GetRelativeRotation();
	float target = current.Pitch + Value;
	if (target > -89 && target < 89)
	{
		SpringArm->AddRelativeRotation(FRotator(Value, 0, 0));
	}
}

void ASpaceship::MoveForward(float Value)
{
	if (CurEnergy <= 0) {
		ForwardValue = 0;
		return;
	}
	ForwardValue = Value;
}

void ASpaceship::MoveRight(float Value)
{
	if (CurEnergy <= 0) {
		RightValue = 0;
		return;
	}
	RightValue = Value;
	const FVector ForwardVector = ShipMesh->GetForwardVector();
	ShipMesh->AddTorqueInRadians(ForwardVector * -Value * 0.1, NAME_None, true);
}

void ASpaceship::Raise(float Value)
{
	if (CurEnergy <= 0) {
		RaiseValue = 0;
		return;
	}
	RaiseValue = Value;
	const FVector RightVector = ShipMesh->GetRightVector();
	ShipMesh->AddTorqueInRadians(RightVector * -Value * 0.1, NAME_None, true);
}

void ASpaceship::TakeOff(float Value)
{
	if (CurEnergy <= 0) {
		UpValue = 0;
		return;
	}
	UpValue = Value;
}

// Called every frame
void ASpaceship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector UpVector = ShipMesh->GetUpVector();
	const FVector ForwardVector = ShipMesh->GetForwardVector();
	const FVector RightVector = ShipMesh->GetRightVector();
	const float MaxPower = (UpValue != 0 && ForwardValue != 0) ? Power * 0.5 : Power;

	// 向上
	if (UpValue > 0 && UpForce < MaxPower)
	{
		UpForce += Power * UpValue * DeltaTime / 10;
		GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, TEXT("UpForce:") + FString::SanitizeFloat(UpForce));
	}
	else if ((UpValue == 0 && UpForce > 0) || UpForce > MaxPower)
	{
		UpForce -= Power * DeltaTime / 10;
		if (UpForce < 0)
		{
			UpForce = 0;
		}
		GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, TEXT("UpForce:") + FString::SanitizeFloat(UpForce));
	}
	
	// 向前or向后
	// 向后的推力只能是20%
	const float MaxRate = ForwardValue < 0 ? 0.2 : 1;
	if ((ForwardValue > 0 && ForwardForce < MaxPower) || (ForwardValue < 0 && ForwardForce > -MaxPower * MaxRate))
	{
		ForwardForce += Power * ForwardValue * DeltaTime / 10;
		GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, TEXT("ForwardForce:") + FString::SanitizeFloat(ForwardForce));
	}
	else if ((ForwardValue == 0 && FMath::Abs(ForwardForce) > 0) || FMath::Abs(ForwardForce) > MaxPower * MaxRate)
	{
		const float ForwardUnit = ForwardForce > 0 ? 1 : -1;
		const float DeltaForce = Power * ForwardUnit * DeltaTime / 10;
		ForwardForce -= DeltaForce;
		if (FMath::Abs(ForwardForce) < FMath::Abs(DeltaForce))
		{
			ForwardForce = 0;
		}
		GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, TEXT("ForwardForce:") + FString::SanitizeFloat(ForwardForce));
	}
	
	float Total = FMath::Abs(UpForce) + FMath::Abs(ForwardForce);
	if (Total > 0)
	{
		ShipMesh->AddForce(UpVector*UpForce + ForwardVector * ForwardForce);
		CurEnergy -= Total * DeltaTime;
		if (CurEnergy < 0)
		{
			CurEnergy = 0;
		}
	}
	GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, TEXT("CurEnergy:") + FString::SanitizeFloat(CurEnergy));
	GEngine->AddOnScreenDebugMessage(3, 1, FColor::Green, TEXT("CurPower:") + FString::SanitizeFloat(Total/Power * 100) + "%");
}

// Called to bind functionality to input
void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &ASpaceship::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpaceship::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASpaceship::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceship::MoveRight);
	PlayerInputComponent->BindAxis("Raise", this, &ASpaceship::Raise);
	PlayerInputComponent->BindAxis("TakeOff", this, &ASpaceship::TakeOff);
}

