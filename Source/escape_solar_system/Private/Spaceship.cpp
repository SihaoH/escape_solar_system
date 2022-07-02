// Copyright 2020 H₂S. All Rights Reserved.

#include "Spaceship.h"
#include "LevelData.h"
#include "MainCharacter.h"
#include "BackpackComponent.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include "MainPlayerState.h"
#include "MainFunctionLibrary.h"
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>

ASpaceship::ASpaceship()
{
	PrimaryActorTick.bCanEverTick = true;

	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ContactTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ContactTrigger"));
	OriginComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OriginComponent"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Backpack = CreateDefaultSubobject<UBackpackComponent>(TEXT("Backpack"));
	Body = CreateDefaultSubobject<UBodyComponent>(TEXT("Body"));
	Engine = CreateDefaultSubobject<UEngineComponent>(TEXT("Engine"));
	
	SetRootComponent(ShipMesh);
	ContactTrigger->SetupAttachment(ShipMesh);
	OriginComponent->SetupAttachment(ShipMesh);
	SpringArm->SetupAttachment(OriginComponent);
	FollowCamera->SetupAttachment(SpringArm);

	ShipMesh->SetSimulatePhysics(true);
	ShipMesh->SetEnableGravity(false);
	ShipMesh->SetLinearDamping(0.05f);
	ShipMesh->SetAngularDamping(0.05f);
	ShipMesh->OnComponentHit.AddUniqueDynamic(Body, &UBodyComponent::OnComponentHitted);
	ContactTrigger->SetBoxExtent(FVector(400.f, 400.f, 180.f), false);
	ContactTrigger->SetEnableGravity(false);
	ContactTrigger->SetMassOverrideInKg(NAME_None, KINDA_SMALL_NUMBER);
	SpringArm->SetRelativeLocation(FVector(0, 0, 220));
	SpringArm->TargetArmLength = 1200;
}

void ASpaceship::SetPilot(AMainCharacter* Pilot)
{
	CurrentPilot = Pilot;
}

void ASpaceship::ResetProperties()
{
	//AMainPlayerState* State = GetController()->GetPlayerState<AMainPlayerState>();
	Backpack->SetBackpack(EPawnType::SpaceShip, LevelBackpack);
	Body->SetStrength(EPawnType::SpaceShip, LevelStrength);
	Body->SetShieldCold(EPawnType::SpaceShip, LevelShieldCold);
	Body->SetShieldHeat(EPawnType::SpaceShip, LevelShieldHeat);
	Body->SetShieldPress(EPawnType::SpaceShip, LevelShieldPress);

	if (LevelEngine2 >= 0)
	{
		EngineType = 2;
	}
	else if (LevelEngine1 >= 0)
	{
		EngineType = 1;
	}
	else
	{
		EngineType = 0;
	}

	switch (EngineType)
	{
	case 0:
		Engine->SetEngine(EPawnType::SpaceShip, LevelEngine0, EngineType);
		Engine->SetEnergy(EPawnType::SpaceShip, LevelEnergy0, EngineType);
		break;
	case 1:
		Engine->SetEngine(EPawnType::SpaceShip, LevelEngine1, EngineType);
		Engine->SetEnergy(EPawnType::SpaceShip, LevelEnergy1, EngineType);
		break;
	case 2:
		Engine->SetEngine(EPawnType::SpaceShip, LevelEngine2, EngineType);
		Engine->SetEnergy(EPawnType::SpaceShip, LevelEnergy2, EngineType);
		break;
	}
}

void ASpaceship::Destroy()
{
	Super::Destroy();
	AMainLevelScriptActor::Instance()->SetSpaceship(nullptr);
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
	PlayerInputComponent->BindAxis("MoveUp", Engine, &UEngineComponent::MoveUp);
}

void ASpaceship::BeginPlay()
{
	Super::BeginPlay();
	AMainLevelScriptActor::Instance()->SetSpaceship(this);

	ResetProperties();
}

void ASpaceship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LookPlanet();
	PerformTurn(DeltaTime);
	PerformAdjust(DeltaTime);
	PerformFOV(DeltaTime);
	UpdateMass();
	//GEngine->AddOnScreenDebugMessage(3, 1, FColor::Green, TEXT("CurPower:") + FString::SanitizeFloat(Total/Power * 100) + "%");
}

void ASpaceship::Controlled()
{
}

void ASpaceship::UnControlled()
{
	OriginComponent->SetRelativeRotation(FRotator::ZeroRotator);
}

void ASpaceship::Thrusting(FVector Force)
{
	ShipMesh->AddForce(Force);
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
	Engine->MoveForward(Value);
}

void ASpaceship::MoveRight(float Value)
{
	if (Engine->GetCurrentEnergy() <= 0 || bFreeLook)
	{
		return;
	}
	OriginComponent->AddRelativeRotation(FRotator(0, 0, Value * 0.2));
}

void ASpaceship::MoveUp(float Value)
{
	Engine->MoveUp(Value);
}

void ASpaceship::PerformTurn(float DeltaTime)
{
	if (Engine->GetCurrentEnergy() <= 0 || bFreeLook) return;

	FRotator DeltaRotation = OriginComponent->GetRelativeRotation() * DeltaTime;
	if (!DeltaRotation.IsNearlyZero(1.e-6f))
	{
		OriginComponent->AddRelativeRotation(DeltaRotation*-1);
		AddActorLocalRotation(DeltaRotation, true);
	}
}

/**
 * 调增飞行姿态，即飞船在世界坐标系中的旋转角度
 * 实现和UGravityMovementComponent::UpdateComponentRotation类似
*/
void ASpaceship::PerformAdjust(float DeltaTime)
{
	GetPlayerState();
	if (!GravityDirection.IsZero() && IsPawnControlled())
	{
		const FVector DesiredUp = GravityDirection * -1.f;
		const FMatrix RotationMatrix = FRotationMatrix::MakeFromZX(DesiredUp, ShipMesh->GetForwardVector());
		const FRotator TargetRotator = FMath::RInterpTo(
			ShipMesh->GetComponentRotation(),
			RotationMatrix.Rotator(),
			DeltaTime,
			GravityAccel*0.001f);
		ShipMesh->SetWorldRotation(TargetRotator, true);
	}
}

void ASpaceship::PerformFOV(float DeltaTime)
{
	float Velocity = GetVelocity().Size();
	float Value = FMath::GetMappedRangeValueClamped(TRange<float>(1000, 10000), TRange<float>(80, 105), Velocity);
	FollowCamera->SetFieldOfView(Value);
}

void ASpaceship::UpdateMass()
{
	const float InMass = Body->GetMass() + Engine->GetTotalMass() + (CurrentPilot ? CurrentPilot->GetMass() : 0);
	if (GetMass() != InMass)
	{
		ShipMesh->SetMassOverrideInKg(NAME_None, InMass);
	}
}
