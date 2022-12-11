// Copyright 2020 H₂S. All Rights Reserved.

#include "Spaceship.h"
#include "TechData.h"
#include "MainCharacter.h"
#include "BackpackComponent.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include "MainPlayerState.h"
#include "MainLibrary.h"
#include "MainLevelScript.h"
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
	SpringArm->SetRelativeLocation(FVector(0, 0, 220));
	SpringArm->TargetArmLength = 1200;
	Body->HpChangedDelegate.AddDynamic(this, &ASpaceship::OnHpChanged);
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
	AMainLevelScript::SetSpaceship(nullptr);
	UMainLibrary::SendMessage(INVTEXT("飞船已销毁"));

	if (CurrentPilot)
	{
		// 如果主角在飞船上，也得死（把血扣光）
		auto Char = CurrentPilot;
		UnDrive();
		Char->Body->ChangeHP(-Char->Body->GetCurrentHP());
	}
}

float ASpaceship::GetGravityAccel() const
{
	return GravityAccel;
}

void ASpaceship::OnHpChanged(float Delta)
{
	if (Body->GetCurrentHP() <= 0)
	{
		Destroy();
	}
}

void ASpaceship::Serialize(FArchive& Ar)
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

void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Hold", IE_Pressed, this, &ASpaceship::UnFreeLook);
	PlayerInputComponent->BindAction("Hold", IE_Released, this, &ASpaceship::FreeLook);
	PlayerInputComponent->BindAction("Drive", IE_Pressed, this, &ASpaceship::UnDrive);
	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &IControllable::LockCelestialBody);

	PlayerInputComponent->BindAxis("Turn", this, &ASpaceship::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpaceship::LookUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASpaceship::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceship::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", Engine, &UEngineComponent::MoveUp);
}

void ASpaceship::BeginPlay()
{
	ContactTrigger->SetBoxExtent(FVector(400.f, 400.f, 180.f), false);
	ContactTrigger->SetEnableGravity(false);
	ContactTrigger->SetMassOverrideInKg(NAME_None, KINDA_SMALL_NUMBER);
	const bool bSimulatePhysics = ShipMesh->GetBodyInstance()->bSimulatePhysics;
	ShipMesh->GetBodyInstance()->bSimulatePhysics = false;
	Super::BeginPlay(); // AActor::BeginPlay()会判断附着的父对象是否模拟物理，如果不一致，就解除附着
	// 所以模拟物理要在BeginPlay之后开启，而且也不能用UPrimitiveComponent::SetSimulatePhysics开启，因为也会像BeginPlay那样做
	ShipMesh->GetBodyInstance()->bSimulatePhysics = bSimulatePhysics;
	ShipMesh->GetBodyInstance()->UpdateInstanceSimulatePhysics();

	AMainLevelScript::SetSpaceship(this);
	ResetProperties();
	Body->ChangeHP(Body->GetMaximumHP());
	Engine->ChangeEnergy(Engine->GetMaximumEnergy());
}

void ASpaceship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckCelestialBody();
	PerformTurn(DeltaTime);
	PerformAdjust(DeltaTime);
	PerformFOV(DeltaTime);
	UpdateMass();
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

void ASpaceship::GravityActedGlobally_Implementation(FVector Direction, float Accel)
{
	ShipMesh->AddForce(Direction * Accel, NAME_None, true);
}

void ASpaceship::BuoyancyActed_Implementation(FVector Force)
{
	ShipMesh->AddForce(Force, NAME_None, true);
}

void ASpaceship::DampingChanged_Implementation(float Linear, float Angular)
{
	FBodyInstance* BI = ShipMesh->GetBodyInstance();
	if (BI)
	{
		BI->LinearDamping = Linear;
		BI->AngularDamping = Angular;
		BI->UpdateDampingProperties();
	}
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
	if (Engine->GetCurrentEnergy() <= 0 || bFreeLook) return;

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
		ShipMesh->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector);
		ShipMesh->GetBodyInstance()->ClearTorques();
		OriginComponent->AddRelativeRotation(DeltaRotation*-1);
		AddActorLocalRotation(DeltaRotation, true);
	}
}

/**
 * 调整飞行姿态，即飞船在世界坐标系中的旋转角度
 * 实现和UGravityMovementComponent::UpdateComponentRotation类似
*/
void ASpaceship::PerformAdjust(float DeltaTime)
{
	if (Engine->GetCurrentEnergy() <= 0 || !bFreeLook) return;

	if (!GravityDirection.IsZero() && IsPawnControlled())
	{
		const FVector DesiredUp = GravityDirection * -1.f;
		// 夹角不相等才调整
		if ((DesiredUp | ShipMesh->GetForwardVector()) < THRESH_NORMALS_ARE_PARALLEL)
		{
			const FMatrix RotationMatrix = FRotationMatrix::MakeFromZX(DesiredUp, ShipMesh->GetForwardVector());
			const FQuat TargetQuat = FMath::QInterpTo(
				ShipMesh->GetComponentQuat(),
				RotationMatrix.ToQuat(),
				DeltaTime,
				GravityAccel * 0.001f);
			ShipMesh->MoveComponent(FVector::ZeroVector, TargetQuat, true);
		}
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
		Density = CalcDensity(InMass, ShipMesh->Bounds.SphereRadius);
	}
}
