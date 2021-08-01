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
	GravityAccel = Accel;
	GravityDirection = Direction;
	ShipMesh->AddForce(GravityDirection * GravityAccel, NAME_None, true);
}

void ASpaceship::Turn(float Value)
{
	SpringArm->AddRelativeRotation(FRotator(0, Value, 0));
}

void ASpaceship::LookUp(float Value)
{
	// 自动修正摄像机视角
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
	if (CurEnergy <= 0)
	{
		return;
	}
	ForwardValue = Value;
}

void ASpaceship::MoveRight(float Value)
{
	if (CurEnergy <= 0)
	{
		return;
	}
	const FVector UpVector = ShipMesh->GetUpVector();
	ShipMesh->AddTorqueInRadians(UpVector * Value * 0.1, NAME_None, true);
}

void ASpaceship::Raise(float Value)
{
	if (CurEnergy <= 0)
	{
		return;
	}
	const FVector RightVector = ShipMesh->GetRightVector();
	ShipMesh->AddTorqueInRadians(RightVector * -Value * 0.1, NAME_None, true);
}

/**
 * 调增飞行姿态，即飞船在世界坐标系中的旋转角度
 * 实现和UGravityMovementComponent::UpdateComponentRotation类似
*/
void ASpaceship::Adjust(float Value)
{
	if (CurEnergy <= 0)
	{
		return;
	}
	if (Value > 0 && !GravityDirection.IsZero())
	{
		const FVector DesiredUp = GravityDirection * -1.f;
		const FMatrix RotationMatrix = FRotationMatrix::MakeFromZX(DesiredUp, ShipMesh->GetForwardVector());
		const FRotator TargetRotator = FMath::RInterpTo(
										ShipMesh->GetComponentRotation(), 
										RotationMatrix.Rotator(), 
										GetWorld()->GetDeltaSeconds(), 
										AdjustingSpeed);
		ShipMesh->SetWorldRotation(TargetRotator);
	}
}

void ASpaceship::TakeOff(float Value)
{
	if (CurEnergy <= 0)
	{
		return;
	}
	UpValue = Value;
}

void ASpaceship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector UpVector = ShipMesh->GetUpVector();
	const FVector ForwardVector = ShipMesh->GetForwardVector();
	const FVector RightVector = ShipMesh->GetRightVector();
	// 前进和起飞都按的话，各方向就只能按50%的功率推进
	const float MaxPower = (UpValue != 0 && ForwardValue != 0) ? Power * 0.5 : Power;

	// 向上or向下
	if (UpValue != 0)
	{
		UpForce = FMath::FInterpConstantTo(UpForce, MaxPower * UpValue, DeltaTime, ThrustingSpeed);
	}
	else if (UpForce != 0)
	{
		UpForce = FMath::FInterpConstantTo(UpForce, 0, DeltaTime, ThrustingSpeed);
	}
	
	// 向前or向后
	if (ForwardValue != 0)
	{
		ForwardForce = FMath::FInterpConstantTo(ForwardForce, MaxPower * ForwardValue, DeltaTime, ThrustingSpeed);
	}
	else if (ForwardForce != 0)
	{
		ForwardForce = FMath::FInterpConstantTo(ForwardForce, 0, DeltaTime, ThrustingSpeed);
	}
	
	float Total = FMath::Abs(UpForce) + FMath::Abs(ForwardForce);
	if (Total > 0)
	{
		//引擎的加速度单位是cm/s²，转换成现实m/s²，需要乘以100
		ShipMesh->AddForce((UpVector*UpForce + ForwardVector*ForwardForce) * 100);
		CurEnergy -= Total * DeltaTime;
		if (CurEnergy < 0)
		{
			CurEnergy = 0;
		}
	}

	UpValue = 0;
	ForwardValue = 0;
	//GEngine->AddOnScreenDebugMessage(3, 1, FColor::Green, TEXT("CurPower:") + FString::SanitizeFloat(Total/Power * 100) + "%");
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
	PlayerInputComponent->BindAxis("Adjust", this, &ASpaceship::Adjust);
	PlayerInputComponent->BindAxis("TakeOff", this, &ASpaceship::TakeOff);
}

