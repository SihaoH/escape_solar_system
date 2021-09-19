// Copyright 2020 H₂S. All Rights Reserved.

#include "MainCharacter.h"
#include "GravityMovementComponent.h"
#include "BackpackComponent.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>

AMainCharacter* AMainCharacter::Instance = nullptr;

AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 若为true，走到球体下方时，会走不动
	bUseControllerRotationYaw = false;

	Movement = Cast<UGravityMovementComponent>(GetCharacterMovement());
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetCapsuleComponent());
	FollowCamera->SetRelativeLocation(FVector(-30.f, 0.f, 60.f));

	Backpack = CreateDefaultSubobject<UBackpackComponent>(TEXT("Backpack"));
}

void AMainCharacter::SetVelocity(const FVector& Velocity)
{
	Movement->Velocity = Velocity;
	// TODO APlayerController::OnPossess的ClientRestart()调用会把速度置0（UMovementComponent::StopMovementImmediately）
}


ASpaceship* AMainCharacter::FindSpaceship() const
{
	ASpaceship* Spaceship = nullptr;
	if (CurrentVehicle)
	{
		Spaceship = CurrentVehicle;
	}
	else
	{
		TArray<AActor*> NearbyActors;
		GetCapsuleComponent()->GetOverlappingActors(NearbyActors, ASpaceship::StaticClass());
		if (NearbyActors.Num() > 0)
		{
			Spaceship = Cast<ASpaceship>(NearbyActors[0]);
		}
	}

	return Spaceship;
}

AEarthBaseActor* AMainCharacter::FindEarthBase() const
{
	TArray<AActor*> NearbyActors;
	GetCapsuleComponent()->GetOverlappingActors(NearbyActors, AEarthBaseActor::StaticClass());
	if (NearbyActors.Num() > 0)
	{
		return Cast<AEarthBaseActor>(NearbyActors[0]);
	}
	return nullptr;
}

void AMainCharacter::Controlled()
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

void AMainCharacter::UnControlled()
{
	SetActorHiddenInGame(true);
	// 该函数会调用UpdateOverlaps，飞船会立即发出碰撞事件，导致NearSpaceship为nullptr
	SetActorEnableCollision(false);
	FollowCamera->SetRelativeRotation(FRotator::ZeroRotator);
	SetVelocity(FVector::ZeroVector);
}

FVector AMainCharacter::GetVelocity() const
{
	FVector Velocity = FVector::ZeroVector;
	FBodyInstance* BodyInst = GetCapsuleComponent()->GetBodyInstance();
	if (BodyInst != NULL)
	{
		Velocity = BodyInst->GetUnrealWorldVelocity();
	}
	return Velocity;
}

void AMainCharacter::GetHP(float & Current, float & Max) const
{
	Current = 88;
	Max = 88;
}

void AMainCharacter::GetMP(float & Current, float & Max) const
{
	Current = 88;
	Max = 88;
}

float AMainCharacter::GetGravityAccel() const
{
	return Movement->GravityAccel;
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &IControllable::LockPlanet);
	PlayerInputComponent->BindAction("Drive", IE_Pressed, this, &AMainCharacter::Drive);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AMainCharacter::MoveUp);
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	Instance = this;
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LookPlanet();
}

void AMainCharacter::GravityActed_Implementation(FVector Direction, float Accel)
{
	Movement->GravityDirection = Direction;
	Movement->GravityAccel = Accel;
}

void AMainCharacter::Drive()
{
	ASpaceship* NearbySpaceship = FindSpaceship();
	if (NearbySpaceship)
	{
		NearbySpaceship->SetPilot(this);
		CurrentVehicle = NearbySpaceship;
		ChangePawn(NearbySpaceship);
	}
}

void AMainCharacter::Turn(float Value)
{
	if (Value == 0) return;

	AddActorLocalRotation(FRotator(0, Value, 0));
}

void AMainCharacter::LookUp(float Value)
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

void AMainCharacter::MoveForward(float Value)
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

void AMainCharacter::MoveRight(float Value)
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

void AMainCharacter::MoveUp(float Value)
{
	if (Value == 0) return;

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Movement->AddForce(Value * Capsule->GetUpVector() * Power * 100);
}
