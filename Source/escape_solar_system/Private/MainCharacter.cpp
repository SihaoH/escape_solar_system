// Copyright 2020 H₂S. All Rights Reserved.

#include "MainCharacter.h"
#include "LevelData.h"
#include "GravityMovementComponent.h"
#include "BackpackComponent.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "PickableItemActor.h"
#include "MainPlayerState.h"
#include "MainLevelScriptActor.h"
#include "MainFunctionLibrary.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>


AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 若为true，走到球体下方时，会走不动
	bUseControllerRotationYaw = false;

	Movement = Cast<UGravityMovementComponent>(GetCharacterMovement());
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Backpack = CreateDefaultSubobject<UBackpackComponent>(TEXT("Backpack"));
	Body = CreateDefaultSubobject<UBodyComponent>(TEXT("Body"));
	Engine = CreateDefaultSubobject<UEngineComponent>(TEXT("Engine"));

	FollowCamera->SetupAttachment(GetCapsuleComponent());
	FollowCamera->SetRelativeLocation(FVector(-30.f, 0.f, 60.f));

	GetCapsuleComponent()->OnComponentHit.AddUniqueDynamic(Body, &UBodyComponent::OnComponentHitted);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMainCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddUniqueDynamic(this, &AMainCharacter::OnEndOverlap);
}

void AMainCharacter::SetVelocity(const FVector& Velocity)
{
	Movement->Velocity = Velocity;
	// TODO APlayerController::OnPossess的ClientRestart()调用会把速度置0（UMovementComponent::StopMovementImmediately）
}

void AMainCharacter::ResetProperties()
{
	//AMainPlayerState* State = GetController()->GetPlayerState<AMainPlayerState>();
	Backpack->SetBackpack(EPawnType::MainChar, LevelBackpack);
	Body->SetStrength(EPawnType::MainChar, LevelStrength);
	Body->SetShieldCold(EPawnType::MainChar, LevelShieldCold);
	Body->SetShieldHeat(EPawnType::MainChar, LevelShieldHeat);
	Body->SetShieldPress(EPawnType::MainChar, LevelShieldPress);

	Engine->SetEngine(EPawnType::MainChar, LevelEngine);
	Engine->SetEnergy(EPawnType::MainChar, LevelEnergy);
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

AEarthBase* AMainCharacter::FindEarthBase() const
{
	TArray<AActor*> NearbyActors;
	GetCapsuleComponent()->GetOverlappingActors(NearbyActors, AEarthBase::StaticClass());
	if (NearbyActors.Num() > 0)
	{
		return Cast<AEarthBase>(NearbyActors[0]);
	}
	return nullptr;
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

float AMainCharacter::GetGravityAccel() const
{
	return Movement->GravityAccel;
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
	// 该函数会调用UpdateOverlaps，飞船会立即发出碰撞事件
	SetActorEnableCollision(false);
	FollowCamera->SetRelativeRotation(FRotator::ZeroRotator);
	SetVelocity(FVector::ZeroVector);
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &IControllable::LockPlanet);
	PlayerInputComponent->BindAction("Drive", IE_Pressed, this, &AMainCharacter::DriveShip);
	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AMainCharacter::PickupItem);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AMainCharacter::MoveUp);
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	AMainLevelScriptActor::GetInstance()->SetMainChar(this);

	ResetProperties();
	Body->ChangeHP(Body->GetMaximumHP() * 0.5);
	Engine->ChangeEnergy(0xFFFF);
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LookPlanet();
	UpdateMass();
}

void AMainCharacter::Thrusting(FVector Force)
{
	Movement->AddForce(Force);
}

void AMainCharacter::GravityActed_Implementation(FVector Direction, float Accel)
{
	Movement->GravityDirection = Direction;
	Movement->GravityAccel = Accel;
}

void AMainCharacter::DriveShip()
{
	ASpaceship* NearbySpaceship = FindSpaceship();
	if (NearbySpaceship)
	{
		NearbySpaceship->SetPilot(this);
		CurrentVehicle = NearbySpaceship;
		ChangePawn(NearbySpaceship);
	}
}

void AMainCharacter::PickupItem()
{
	if (PickableItem)
	{
		FName RowName;
		int32 PickedCount = 0;
		PickableItem->Pickup(RowName, PickedCount);
		int32 AddedCount = FMath::Min(Backpack->GetMaxAddNum(RowName), PickedCount);
		Backpack->AddItem(RowName, AddedCount);

		UMainFunctionLibrary::SendMessage(FText::Format(
			INVTEXT("拾取了 {0} x{1}"),
			UMainFunctionLibrary::GetItemData(RowName).Name, 
			AddedCount)
		);
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
	if (!Movement->IsFalling())
	{
		AddMovementInput(GetActorForwardVector(), Value);
		Engine->MoveForward(0);
	}
	else
	{
		Engine->MoveForward(Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
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
	Engine->MoveUp(Value);
}

void AMainCharacter::UpdateMass()
{
	const float InMass = Body->GetMass() + Engine->GetTotalMass() + Backpack->GetMass();
	if (Movement->Mass != InMass)
	{
		Movement->Mass = InMass;
	}
}

void AMainCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APickableItemActor* ItemActor = Cast<APickableItemActor>(OtherActor);
	if (ItemActor)
	{
		PickableItem = ItemActor;
	}
}

void AMainCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APickableItemActor* ItemActor = Cast<APickableItemActor>(OtherActor);
	if (ItemActor == PickableItem)
	{
		PickableItem = nullptr;
	}
}
