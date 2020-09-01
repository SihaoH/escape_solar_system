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

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetCapsuleComponent());
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

}

void AGravityCharacter::Turn(float Value)
{
	AddActorLocalRotation(FRotator(0, Value, 0));
}

void AGravityCharacter::LookUp(float Value)
{
	Value = Value * -1;
	FRotator current = FollowCamera->GetRelativeRotation();
	float target = current.Pitch + Value;
	if (target > -89 && target < 89)
	{
		FollowCamera->AddRelativeRotation(FRotator(Value, 0, 0));
	}
}

void AGravityCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AGravityCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

