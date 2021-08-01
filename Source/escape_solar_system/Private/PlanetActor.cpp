// Copyright 2020 H₂S. All Rights Reserved.


#include "PlanetActor.h"
#include "MassActorInterface.h"
#include "Components/SphereComponent.h"

APlanetActor::APlanetActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GravityZone = CreateDefaultSubobject<USphereComponent>(TEXT("GravityZone"));
	GravityZone->SetupAttachment(GetStaticMeshComponent());
	GravityZone->OnComponentEndOverlap.AddDynamic(this, &APlanetActor::OnComponentEndOverlap);
}

/** 
 * 默认实现需要开启[模拟物理]才能获取到速度
 * 但开启[模拟物理]后会和各种物体（包括角色）产生物理效果
 * 而星球本身是要保持稳定的，只能按自转和公转来运动，所以重载该函数
 * 实现和UPrimitiveComponent::GetComponentVelocity类似，只是去掉了[模拟物理]的判断
 */
FVector APlanetActor::GetVelocity() const
{
	FVector Velocity = FVector::ZeroVector;
	FBodyInstance* BodyInst = GetStaticMeshComponent()->GetBodyInstance();
	if (BodyInst != NULL)
	{
		Velocity = BodyInst->GetUnrealWorldVelocity();
	}
	return Velocity;
}

void APlanetActor::BeginPlay()
{
	Super::BeginPlay();
	DistanceRadius = FVector::Distance(CentralActor->GetActorLocation(), GetActorLocation());
}

void APlanetActor::Tick(float DeltaTime)
{
	PerformRotation(DeltaTime);
	PerformRevolution(DeltaTime);
	PerformGravity(DeltaTime);
	Super::Tick(DeltaTime);
}

void APlanetActor::PerformRotation(float DeltaTime)
{
	AddActorLocalRotation(FRotator(0, RotationSpeed * DeltaTime, 0));
}

void APlanetActor::PerformRevolution(float DeltaTime)
{
	FVector CentralLocation = CentralActor->GetActorLocation();
	FVector Location = GetActorLocation();
	Location = Location - CentralLocation;
	float Angle = FMath::Acos(Location.CosineAngle2D(FVector(1.0f, 0, 0))) * FMath::Sign(Location.Y);
	float NewAngle = Angle + FMath::DegreesToRadians( RevolutionSpeed * DeltaTime );
	SetActorLocation(FVector(FMath::Cos(NewAngle) * DistanceRadius, FMath::Sin(NewAngle) * DistanceRadius, 0) + CentralLocation);
}

void APlanetActor::PerformGravity(float DeltaTime)
{
	const FVector SelfLocation = GetActorLocation();

	TSet<AActor*> OverlappingActors;
	GravityZone->GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
		{
			const FVector TargetLocation = Actor->GetActorLocation();
			FVector Direction = (SelfLocation - TargetLocation).GetSafeNormal();
			float MeterDistance = FVector::Distance(SelfLocation, TargetLocation) / 100;
			float Accel = (6.67f * Mass) / FMath::Square(MeterDistance);
			IMassActorInterface::Execute_GravityActed(Actor, Direction, Accel);
		}
	}
}

void APlanetActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
	{
		IMassActorInterface::Execute_GravityActed(OtherActor, FVector::ZeroVector, 0.f);
	}
}
