// Copyright 2020 H₂S. All Rights Reserved.

#include "CelestialBody.h"
#include "MassActorInterface.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SphereComponent.h>


ACelestialBody::ACelestialBody()
{
	PrimaryActorTick.bCanEverTick = true;
	// 这项很重要，在关卡开始时，已经放置在碰撞区域内的，可以发出一次BeginOverlap事件
	bGenerateOverlapEventsDuringLevelStreaming = true;
	RootComponent = GetStaticMeshComponent();
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	GravityZone = CreateDefaultSubobject<USphereComponent>(TEXT("GravityZone"));
	GravityZone->SetGenerateOverlapEvents(true);
	// 必须附着在RootComponent上，否则无法获取到重叠物品
	GravityZone->SetupAttachment(GetRootComponent());
	GravityZone->OnComponentBeginOverlap.AddDynamic(this, &ACelestialBody::OnGravityZoneBeginOverlap);
	GravityZone->OnComponentEndOverlap.AddDynamic(this, &ACelestialBody::OnGravityZoneEndOverlap);
}

/** 
 * 默认实现需要开启[模拟物理]才能获取到速度
 * 但开启[模拟物理]后会和各种物体（包括角色）产生物理效果
 * 而星球本身是要保持稳定的，只能按自转和公转来运动，所以重载该函数
 * 实现和UPrimitiveComponent::GetComponentVelocity类似，只是去掉了[模拟物理]的判断
 */
FVector ACelestialBody::GetVelocity() const
{
	FVector Velocity = FVector::ZeroVector;
	FBodyInstance* BodyInst = GetStaticMeshComponent()->GetBodyInstance();
	if (BodyInst != NULL)
	{
		Velocity = BodyInst->GetUnrealWorldVelocity();
	}
	return Velocity;
}

void ACelestialBody::CalcGravityResult(AActor* Target, FVector& Direction, float& Accel) const
{
	const FVector SelfLocation = GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();
	Direction = (SelfLocation - TargetLocation).GetSafeNormal();
	float Mass = SurfaceGravity * FMath::Square(SelfRadius);
	float Distance = FVector::Dist(SelfLocation, TargetLocation);
	Accel = Mass / FMath::Square(Distance);
}

void ACelestialBody::Serialize(FArchive& Ar)
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
	}
	else
	{
		Super::Serialize(Ar);
	}
}

void ACelestialBody::BeginPlay()
{
	Super::BeginPlay();
	if (SelfRadius <= 0)
	{
		SelfRadius = GetStaticMeshComponent()->Bounds.SphereRadius;
	}
	if (GetAttachParentActor())
	{
		DistanceRadius = FVector::Dist(GetAttachParentActor()->GetActorLocation(), GetActorLocation());
	}
}

void ACelestialBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformRotation(DeltaTime);
	PerformGravity(DeltaTime);
}

void ACelestialBody::PerformRotation(float DeltaTime)
{
	if (RotationSpeed != 0)
	{
		AddActorLocalRotation(FRotator(0, RotationSpeed * DeltaTime, 0));
	}
}

/**
 * 对[质量体]产生引力作用
 * 在星球表面时引力最大，往外飞或往内部钻，引力都会越来越小
 */
void ACelestialBody::PerformGravity(float DeltaTime)
{
	TSet<AActor*> OverlappingActors;
	GravityZone->GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
		{
			FVector Direction = FVector::ZeroVector;
			float Accel = 0;
			CalcGravityResult(Actor, Direction, Accel);

			IMassActorInterface::Execute_GravityActed(Actor, Direction, Accel);
		}
	}
}

void ACelestialBody::OnGravityZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		auto BI = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent())->GetBodyInstance();
		bool bSimulate = BI->bSimulatePhysics;
		BI->bSimulatePhysics = false;
		OtherActor->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		BI->bSimulatePhysics = bSimulate;
		BI->UpdateInstanceSimulatePhysics();

		if (OtherActor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
		{
			Cast<IMassActorInterface>(OtherActor)->PlanetOwner = this;
		}
	}
}

void ACelestialBody::OnGravityZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OtherActor->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	if (OtherActor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
	{
		Cast<IMassActorInterface>(OtherActor)->PlanetOwner = nullptr;
		IMassActorInterface::Execute_GravityActed(OtherActor, FVector::ZeroVector, 0.f);
	}
}
