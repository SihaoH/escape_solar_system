// Copyright 2020 H₂S. All Rights Reserved.

#include "MainCharacter.h"
#include "TechData.h"
#include "GravityMovementComponent.h"
#include "BackpackComponent.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "PickableItem.h"
#include "NPC.h"
#include "MainPlayerState.h"
#include "MainLevelScript.h"
#include "MainLibrary.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraActor.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#define LOCTEXT_NAMESPACE "MainCharacter"


ACameraActor* AMainCharacter::DeathCamera = nullptr;

AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 必须设为false，否则走到球体底部时会走不动
	bUseControllerRotationYaw = false;

	Movement = Cast<UGravityMovementComponent>(GetCharacterMovement());
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Backpack = CreateDefaultSubobject<UBackpackComponent>(TEXT("Backpack"));
	Body = CreateDefaultSubobject<UBodyComponent>(TEXT("Body"));
	Engine = CreateDefaultSubobject<UEngineComponent>(TEXT("Engine"));

	FollowCamera->SetupAttachment(GetCapsuleComponent());
	FollowCamera->SetRelativeLocation(FVector(-30.f, 0.f, 60.f));

	Body->HpChangedDelegate.AddDynamic(this, &AMainCharacter::OnHpChanged);
	GetCapsuleComponent()->OnComponentHit.AddUniqueDynamic(Body, &UBodyComponent::OnComponentHitted);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMainCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddUniqueDynamic(this, &AMainCharacter::OnEndOverlap);
}

void AMainCharacter::SetVelocity(const FVector& Velocity)
{
	Movement->Velocity = Velocity;
	// TODO APlayerController::OnPossess的ClientRestart()调用会把速度置0（UMovementComponent::StopMovementImmediately）
}

FText AMainCharacter::GetDisplayName() const
{
	return LOCTEXT("Body", "躯体");
}

void AMainCharacter::ResetProperties()
{
	//AMainPlayerState* State = GetController()->GetPlayerState<AMainPlayerState>();
	Body->SetStrength(EPawnType::MainChar, LevelStrength);
	Backpack->SetBackpack(EPawnType::MainChar, LevelBackpack);
	Body->SetShieldCold(EPawnType::MainChar, LevelShieldCold);
	Body->SetShieldHeat(EPawnType::MainChar, LevelShieldHeat);
	Body->SetShieldPress(EPawnType::MainChar, LevelShieldPress);

	Engine->SetEngine(EPawnType::MainChar, LevelEngine);
	Engine->SetEnergy(EPawnType::MainChar, LevelEnergy);
}

void AMainCharacter::Destroy()
{
	Super::Destroy();
	ChangePawn(nullptr);
	AMainLevelScript::SetMainChar(nullptr);

	// Actor销毁后，画面虽然还留在该Actor上，但视角（旋转）会归零，所以要创建一个保留死亡视角的摄像机
	if (DeathCamera)
	{
		DeathCamera->TeleportTo(FollowCamera->GetComponentLocation(), FollowCamera->GetComponentRotation());
	}
	else
	{
		DeathCamera = GetWorld()->SpawnActor<ACameraActor>(FollowCamera->GetComponentLocation(), FollowCamera->GetComponentRotation());
		DeathCamera->GetCameraComponent()->SetConstraintAspectRatio(false);
	}
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetViewTargetWithBlend(DeathCamera);
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

void AMainCharacter::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving())
	{
		SavedTransform = GetActorTransform();
		Super::Serialize(Ar);
	}
	else if (Ar.IsLoading())
	{
		Super::Serialize(Ar);
		SetActorTransform(SavedTransform);
		ResetProperties();
	}
	else
	{
		Super::Serialize(Ar);
	}
}

void AMainCharacter::Controlled()
{
	if (CurrentVehicle)
	{
		const FVector TargetLocation = CurrentVehicle->GetActorLocation() + CurrentVehicle->GetRootComponent()->GetRightVector() * 200;
		const FRotator TargetRotation = CurrentVehicle->GetActorRotation();
		const FVector TargetVelocity = CurrentVehicle->GetVelocity();

		FollowCamera->SetRelativeRotation(FRotator::ZeroRotator);
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
	SetVelocity(FVector::ZeroVector);
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &IControllable::LockPlanet);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AMainCharacter::MoveUp);
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	AMainLevelScript::SetMainChar(this);
	ChangePawn(this);

	ResetProperties();
	Body->ChangeHP(9999);
	Engine->ChangeEnergy(Engine->GetMaximumEnergy());
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMass();
	LookPlanet();
	CheckPickup();
	CheckNPC();
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

void AMainCharacter::BuoyancyActed_Implementation(FVector Force)
{
	Movement->AddForce(Force * Movement->Mass);
}

void AMainCharacter::DampingChanged_Implementation(float Linear, float Angular)
{
	Movement->BrakingFriction = Linear;
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

		UMainLibrary::SendMessage(FText::Format(
			LOCTEXT("Pickup", "拾取了 {0} x{1}"),
			UMainLibrary::GetItemData(RowName).Name, 
			AddedCount)
		);
	}
}

void AMainCharacter::TalkToNPC()
{
	AMainLevelScript::Instance()->TalkOpenedDelegate.Broadcast();
}

void AMainCharacter::Turn(float Value)
{
	if (Value == 0) return;
	
	if (!GetPlanetOwner())
	{
		// 要把摄像头的角度回正，不然躯体转向会导致视角异常
		auto Angle = FollowCamera->GetRelativeRotation().Pitch;
		if (FMath::Abs(Angle) > 1)
		{
			FollowCamera->AddRelativeRotation(FRotator(-FMath::Sign(Angle) * 0.1, 0, 0));
		}
	}

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
		auto Angle = FollowCamera->GetRelativeRotation().Pitch;
		if (FMath::Abs(Angle) > 1)
		{
			FollowCamera->AddRelativeRotation(FRotator(-FMath::Sign(Angle) * 0.1, 0, 0));
		}
		else
		{
			AddActorLocalRotation(FRotator(Value, 0, 0));
		}
	}
}

void AMainCharacter::MoveForward(float Value)
{
	if (!Movement->IsFalling())
	{
		AddMovementInput(GetActorForwardVector(), Value);
		Engine->MoveForward(0);
		Movement->AddForce(GetActorForwardVector()*Movement->Mass*100);
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

		// 胶囊体，半径计算要更小一点
		Density = CalcDensity(InMass, GetCapsuleComponent()->Bounds.SphereRadius / 2);
	}
}

void AMainCharacter::CheckPickup()
{
	UPickableItem* NewPickupItem = nullptr;
	if (Controller && Controller->GetPawn() == this)
	{
		NewPickupItem = FindByLineTrace<UPickableItem>(250.f);
	}

	if (NewPickupItem != PickableItem)
	{
		if (NewPickupItem)
		{
			auto ItemRowName = NewPickupItem->ItemRowName;
			NewPickupItem->SetHighlight(true);
			AMainLevelScript::AddActionPrompt("Pickup", UMainLibrary::GetItemData(ItemRowName).Name, 2)
				.AddUniqueDynamic(this, &AMainCharacter::PickupItem);
		}
		else
		{
			PickableItem->SetHighlight(false);
			AMainLevelScript::RemoveActionPrompt("Pickup");
		}
		PickableItem = NewPickupItem;
	}
}

void AMainCharacter::CheckNPC()
{
	ANPC* NewNPC = nullptr;
	if (Controller && Controller->GetPawn() == this)
	{
		NewNPC = FindByLineTrace<ANPC>(250.f);
	}

	if (NewNPC != TalkableNPC)
	{
		if (NewNPC)
		{
			NewNPC->SetHighlight(true);
			AMainLevelScript::AddActionPrompt("Talk", NewNPC->GetName())
				.AddUniqueDynamic(this, &AMainCharacter::TalkToNPC);
		}
		else
		{
			TalkableNPC->SetHighlight(false);
			AMainLevelScript::RemoveActionPrompt("Talk");
		}
		TalkableNPC = NewNPC;
	}
}

void AMainCharacter::OnHpChanged(float Delta)
{
	if (Body->GetCurrentHP() <= 0)
	{
		Destroy();
	}
}

void AMainCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ASpaceship>())
	{
		AMainLevelScript::AddActionPrompt("Drive", LOCTEXT("Drive", "登上飞船"))
			.AddUniqueDynamic(this, &AMainCharacter::DriveShip);
	}
	else if (OtherActor->IsA<AEarthBase>())
	{
		// _Menu和Menu是一样的按键，但_Menu是特意为了提示用的，实际还是Menu起作用
		AMainLevelScript::AddActionPrompt("_Menu", LOCTEXT("Menu", "连接基地"));
	}
}

void AMainCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<ASpaceship>())
	{
		AMainLevelScript::RemoveActionPrompt("Drive");
	}
	else if (OtherActor->IsA<AEarthBase>())
	{
		AMainLevelScript::RemoveActionPrompt("_Menu");
	}
}

#undef LOCTEXT_NAMESPACE
