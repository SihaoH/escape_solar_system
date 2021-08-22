// Copyright 2020 H₂S. All Rights Reserved.

#include "GravityCharacter.h"
#include "GravityMovementComponent.h"
#include "Spaceship.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>

// Sets default values
AGravityCharacter::AGravityCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 若为true，走到球体下方时，会走不动
	bUseControllerRotationYaw = false;

	Movement = Cast<UGravityMovementComponent>(GetCharacterMovement());
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetCapsuleComponent());
}

void AGravityCharacter::SetVelocity(const FVector& Velocity)
{
	Movement->Velocity = Velocity;
	// TODO APlayerController::OnPossess的ClientRestart()调用会把速度置0（UMovementComponent::StopMovementImmediately）
}

void AGravityCharacter::SetNearSpaceship(APawn* Spaceship)
{
	NearSpaceship = Spaceship;
}

void AGravityCharacter::Controlled()
{
	if (CurrentVehicle)
	{
		const FVector TargetLocation = CurrentVehicle->GetActorLocation() + CurrentVehicle->GetRootComponent()->GetRightVector() * 200;
		const FRotator TargetRotation = CurrentVehicle->GetActorRotation();
		const FVector TargetVelocity = CurrentVehicle->GetVelocity();
		
		TeleportTo(TargetLocation, TargetRotation);
		SetVelocity(TargetVelocity);
	}
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	CurrentVehicle = nullptr;
}

void AGravityCharacter::UnControlled()
{
	SetActorHiddenInGame(true);
	// 该函数会调用UpdateOverlaps，飞船会立即发出碰撞事件，导致NearSpaceship为nullptr
	SetActorEnableCollision(false);
	FollowCamera->SetRelativeRotation(FRotator::ZeroRotator);
	SetVelocity(FVector::ZeroVector);
}

FVector AGravityCharacter::GetVelocity() const
{
	FVector Velocity = FVector::ZeroVector;
	FBodyInstance* BodyInst = GetCapsuleComponent()->GetBodyInstance();
	if (BodyInst != NULL)
	{
		Velocity = BodyInst->GetUnrealWorldVelocity();
	}
	return Velocity;
}

void AGravityCharacter::GetHP(float & Current, float & Max) const
{
	Current = 88;
	Max = 88;
}

void AGravityCharacter::GetMP(float & Current, float & Max) const
{
	Current = 88;
	Max = 88;
}

float AGravityCharacter::GetGravityAccel() const
{
	return Movement->GravityAccel;
}

void AGravityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &IControllable::LockPlanet);
	PlayerInputComponent->BindAction("Drive", IE_Pressed, this, &AGravityCharacter::Drive);

	PlayerInputComponent->BindAxis("Turn", this, &AGravityCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AGravityCharacter::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGravityCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGravityCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AGravityCharacter::MoveUp);
}

void AGravityCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGravityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LookPlanet();
}

void AGravityCharacter::GravityActed_Implementation(FVector Direction, float Accel)
{
	Movement->GravityDirection = Direction;
	Movement->GravityAccel = Accel;
}

void AGravityCharacter::Drive()
{
	if (NearSpaceship)
	{
		Cast<ASpaceship>(NearSpaceship)->SetPilot(this);
		CurrentVehicle = NearSpaceship;
		ChangePawn(NearSpaceship);
	}
}

void AGravityCharacter::Turn(float Value)
{
	if (Value == 0) return;

	AddActorLocalRotation(FRotator(0, Value, 0));
}

void AGravityCharacter::LookUp(float Value)
{
	if (Value == 0) return;

	Value = Value * -1;
	if (GetPlanetOwner())
	{
		FRotator current = FollowCamera->GetRelativeRotation();
		float target = current.Pitch + Value;
		if (target > -89 && target < 89)
		{
			FollowCamera->AddRelativeRotation(FRotator(Value, 0, 0));
		}
	}
	else
	{
		AddActorLocalRotation(FRotator(Value, 0, 0));
	}
}

void AGravityCharacter::MoveForward(float Value)
{
	if (Value == 0) return;

	if (GetPlanetOwner())
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
	else
	{
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		Movement->AddForce(Value * Capsule->GetForwardVector() * Power * 100);
	}
}

void AGravityCharacter::MoveRight(float Value)
{
	if (Value == 0) return;

	if (GetPlanetOwner())
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
	else
	{
		AddActorLocalRotation(FRotator(0, 0, Value * 0.2));
	}
}

void AGravityCharacter::MoveUp(float Value)
{
	if (Value == 0) return;

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Movement->AddForce(Value * Capsule->GetUpVector() * Power * 100);
}

