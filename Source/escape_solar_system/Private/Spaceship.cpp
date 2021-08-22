// Copyright 2020 H₂S. All Rights Reserved.

#include "Spaceship.h"
#include "GravityCharacter.h"
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>


ASpaceship::ASpaceship(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ContactTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ContactTrigger"));
	OriginComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OriginComponent"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	
	SetRootComponent(ShipMesh);
	ContactTrigger->SetupAttachment(ShipMesh);
	OriginComponent->SetupAttachment(ShipMesh);
	SpringArm->SetupAttachment(OriginComponent);
	FollowCamera->SetupAttachment(SpringArm);

	ShipMesh->SetSimulatePhysics(true);
	ShipMesh->SetEnableGravity(false);
	ShipMesh->SetLinearDamping(0.05f);
	ShipMesh->SetAngularDamping(0.05f);
	ShipMesh->SetMassOverrideInKg(NAME_None, 200.f); //TODO 飞船重量是可变值，暂时先用200kg
	ContactTrigger->SetBoxExtent(FVector(400.f, 400.f, 180.f), false);
	ContactTrigger->SetEnableGravity(false);
	ContactTrigger->SetMassOverrideInKg(NAME_None, 0.001f);
	SpringArm->SetRelativeLocation(FVector(0, 0, 220));
	SpringArm->TargetArmLength = 1200;

	ContactTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASpaceship::OnSomethingClosed);
	ContactTrigger->OnComponentEndOverlap.AddDynamic(this, &ASpaceship::OnSomethingLeft);
}

void ASpaceship::SetPilot(APawn* Pilot)
{
	CurrentPilot = Pilot;
}

void ASpaceship::GetHP(float & Current, float & Max) const
{
	Current = 99;
	Max = 99;
}

void ASpaceship::GetMP(float & Current, float & Max) const
{
	Current = CurEnergy;
	Max = MaxEnergy;
}

float ASpaceship::GetGravityAccel() const
{
	return GravityAccel;
}

void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Hold", IE_Pressed, this, &ASpaceship::UnFreeLook);
	PlayerInputComponent->BindAction("Hold", IE_Released, this, &ASpaceship::FreeLook);
	PlayerInputComponent->BindAction("Drive", IE_Pressed, this, &ASpaceship::UnDrive);
	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &IControllable::LockPlanet);

	PlayerInputComponent->BindAxis("Turn", this, &ASpaceship::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpaceship::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASpaceship::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceship::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ASpaceship::MoveUp);
}

void ASpaceship::BeginPlay()
{
	Super::BeginPlay();
}

void ASpaceship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LookPlanet();
	PerformThrust(DeltaTime);
	PerformTurn(DeltaTime);
	PerformAdjust(DeltaTime);
	PerformFOV(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(3, 1, FColor::Green, TEXT("CurPower:") + FString::SanitizeFloat(Total/Power * 100) + "%");
}

void ASpaceship::Controlled()
{
}

void ASpaceship::UnControlled()
{
	OriginComponent->SetRelativeRotation(FRotator::ZeroRotator);
}

void ASpaceship::GravityActed_Implementation(FVector Direction, float Accel)
{
	GravityAccel = Accel;
	GravityDirection = Direction;
	ShipMesh->AddForce(GravityDirection * GravityAccel, NAME_None, true);
}

void ASpaceship::Turn(float Value)
{
	OriginComponent->AddRelativeRotation(FRotator(0, Value, 0));
}

void ASpaceship::LookUp(float Value)
{
	Value = Value * -1;
	FRotator Current = OriginComponent->GetRelativeRotation();
	float Target = Current.Pitch + Value;
	if (Target > -89 && Target < 89)
	{
		OriginComponent->AddRelativeRotation(FRotator(Value, 0, 0));
	}
}

void ASpaceship::FreeLook()
{
	bFreeLook = true;
}

void ASpaceship::UnFreeLook()
{
	bFreeLook = false;
}

void ASpaceship::UnDrive()
{
	if (CurrentPilot)
	{
		ChangePawn(CurrentPilot);
		SetPilot(nullptr);
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
	if (CurEnergy <= 0 || bFreeLook)
	{
		return;
	}
	OriginComponent->AddRelativeRotation(FRotator(0, 0, Value * 0.2));
}

void ASpaceship::MoveUp(float Value)
{
	if (CurEnergy <= 0)
	{
		return;
	}
	UpValue = Value;
}

void ASpaceship::PerformTurn(float DeltaTime)
{
	if (CurEnergy <= 0 || bFreeLook) return;

	FRotator DeltaRotation = OriginComponent->GetRelativeRotation() * DeltaTime;
	if (!DeltaRotation.IsNearlyZero(1.e-6f))
	{
		OriginComponent->AddRelativeRotation(DeltaRotation*-1);
		AddActorLocalRotation(DeltaRotation, true);
	}
}

void ASpaceship::PerformThrust(float DeltaTime)
{
	const FVector UpVector = ShipMesh->GetUpVector();
	const FVector ForwardVector = ShipMesh->GetForwardVector();
	const FVector RightVector = ShipMesh->GetRightVector();

	// 上下
	if (UpValue != 0)
	{
		UpForce = FMath::FInterpConstantTo(UpForce, Power * UpValue, DeltaTime, ThrustingSpeed);
	}
	else if (UpForce != 0)
	{
		UpForce = FMath::FInterpConstantTo(UpForce, 0, DeltaTime, ThrustingSpeed);
	}

	// 前后
	if (ForwardValue != 0)
	{
		ForwardForce = FMath::FInterpConstantTo(ForwardForce, Power * ForwardValue, DeltaTime, ThrustingSpeed);
	}
	else if (ForwardForce != 0)
	{
		ForwardForce = FMath::FInterpConstantTo(ForwardForce, 0, DeltaTime, ThrustingSpeed);
	}

	float Total = FMath::Abs(UpForce) + FMath::Abs(ForwardForce) ;
	if (Total > 0)
	{
		// F=ma，a在UI上显示的是m/s²，而引擎中使用的是cm/s²，转换需要乘以100
		ShipMesh->AddForce((UpVector*UpForce + ForwardVector*ForwardForce) * 100);
		CurEnergy -= Total * DeltaTime;
		if (CurEnergy < 0)
		{
			CurEnergy = 0;
		}
	}

	UpValue = 0;
	ForwardValue = 0;
}

/**
 * 调增飞行姿态，即飞船在世界坐标系中的旋转角度
 * 实现和UGravityMovementComponent::UpdateComponentRotation类似
*/
void ASpaceship::PerformAdjust(float DeltaTime)
{
	if (!GravityDirection.IsZero())
	{
		const FVector DesiredUp = GravityDirection * -1.f;
		const FMatrix RotationMatrix = FRotationMatrix::MakeFromZX(DesiredUp, ShipMesh->GetForwardVector());
		const FRotator TargetRotator = FMath::RInterpTo(
			ShipMesh->GetComponentRotation(),
			RotationMatrix.Rotator(),
			DeltaTime,
			GravityAccel*0.001);
		ShipMesh->SetWorldRotation(TargetRotator, true);
	}
}

void ASpaceship::PerformFOV(float DeltaTime)
{
	float Velocity = GetVelocity().Size();
	float Value = FMath::GetMappedRangeValueClamped(TRange<float>(1000, 10000), TRange<float>(80, 105), Velocity);
	FollowCamera->SetFieldOfView(Value);
}

void ASpaceship::OnSomethingClosed(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	AGravityCharacter* MainCharacter = Cast<AGravityCharacter>(OtherActor);
	if (MainCharacter)
	{
		MainCharacter->SetNearSpaceship(this);
	}
}

void ASpaceship::OnSomethingLeft(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
	AGravityCharacter* MainCharacter = Cast<AGravityCharacter>(OtherActor);
	if (MainCharacter)
	{
		MainCharacter->SetNearSpaceship(nullptr);
	}
}
