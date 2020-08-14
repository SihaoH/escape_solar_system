// Copyright 2020 H2S. All Rights Reserved.

#include "GravityCharacter.h"
#include "GravityMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AGravityCharacter::AGravityCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 若为true，走到球体下方时，会走不动
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	FollowCamera->SetupAttachment(CameraBoom);
}

// Called to bind functionality to input
void AGravityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Turn", this, &AGravityCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AGravityCharacter::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGravityCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGravityCharacter::MoveRight);
}

// Called when the game starts or when spawned
void AGravityCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGravityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMoveForward)
	{
		float offset = CameraBoom->GetRelativeRotation().Yaw;
		if (FMath::Abs(offset) > 1 && FMath::Abs(offset) > FMath::Abs(YawOffset))
		{
			YawOffset = offset;
		}
		else if (FMath::Abs(offset) <= 1)
		{
			YawOffset = 0;
		}

		if (YawOffset != 0)
		{
			float delta = YawOffset * DeltaTime * 5;
			AddActorLocalRotation(FRotator(0, delta, 0));
			CameraBoom->AddRelativeRotation(FRotator(0, -delta, 0));
		}
	}
	else {
		YawOffset = 0;
	}
}

void AGravityCharacter::Turn(float Value)
{
	if (bMoveForward)
	{
		AddActorLocalRotation(FRotator(0, Value, 0));
	}
	else 
	{
		CameraBoom->AddRelativeRotation(FRotator(0, Value, 0));
	}
}

void AGravityCharacter::LookUp(float Value)
{
	Value = Value * -1;
	FRotator current = CameraBoom->GetRelativeRotation();
	float target = current.Pitch + Value;
	if (target > -89 && target < 89) 
	{
		CameraBoom->AddRelativeRotation(FRotator(Value, 0, 0));
	}
}

void AGravityCharacter::MoveForward(float Value)
{
	bMoveForward = (Value != 0);
	AddMovementInput(GetActorForwardVector(), Value);
}

void AGravityCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

