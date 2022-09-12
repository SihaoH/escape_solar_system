﻿// Copyright 2020 H₂S. All Rights Reserved.

#include "PlanetActor.h"
#include "MassActorInterface.h"
#include "../UI/PlanetInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

APlanetActor::APlanetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	// 这项很重要，在关卡开始时，已经放置在碰撞区域内的，可以发出一次BeginOverlap事件
	bGenerateOverlapEventsDuringLevelStreaming = true;
	RootComponent = GetStaticMeshComponent();
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	GravityZone = CreateDefaultSubobject<USphereComponent>(TEXT("GravityZone"));
	// 必须附着在RootComponent上，否则无法获取到重叠物品
	GravityZone->SetupAttachment(GetRootComponent());
	GravityZone->OnComponentBeginOverlap.AddDynamic(this, &APlanetActor::OnGravityZoneBeginOverlap);
	GravityZone->OnComponentEndOverlap.AddDynamic(this, &APlanetActor::OnGravityZoneEndOverlap);

	InfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidget"));
	// 必须附着在RootComponent上，否则会显示在世界的中心，而不是该星球的中心
	InfoWidget->SetupAttachment(GetRootComponent());
	InfoWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InfoWidget->SetDrawSize({ 0, 0 });
	InfoWidget->SetWidgetClass(LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UI/WB_PlanetInfo.WB_PlanetInfo_C'")));
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

void APlanetActor::SetLooked(bool Looked)
{
	InfoWidgetObject->SetLooked(Looked);
}

void APlanetActor::SetLocked(bool Locked)
{
	InfoWidgetObject->SetLocked(Locked);
}

void APlanetActor::CalcGravityResult(AActor* Target, FVector& Direction, float& Accel) const
{
	const FVector SelfLocation = GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();
	Direction = (SelfLocation - TargetLocation).GetSafeNormal();
	float Distance = FVector::Dist(SelfLocation, TargetLocation) - SelfRadius;
	Accel = SurfaceGravity * FMath::Max((1 - Distance / 3 / SelfRadius), 0.f);
}

void APlanetActor::Serialize(FArchive& Ar)
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

void APlanetActor::BeginPlay()
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

	InfoWidgetObject = Cast<UPlanetInfo>(InfoWidget->GetUserWidgetObject());
	InfoWidgetObject->SetName(Name, Icon);
	GetWorldTimerManager().SetTimer(InfoTimer, this, &APlanetActor::UpdateInfoWidget, 0.1f, true, 0.f);
}

void APlanetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformRotation(DeltaTime);
	PerformGravity(DeltaTime);
}

void APlanetActor::PerformRotation(float DeltaTime)
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
void APlanetActor::PerformGravity(float DeltaTime)
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

void APlanetActor::UpdateInfoWidget()
{
	const APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Player)
	{
		float Distance = FVector::Dist(Player->GetActorLocation(), GetActorLocation()) - SelfRadius;
		if (PreDistance != 0)
		{
			float Delta = (Distance - PreDistance) / 0.1;
			InfoWidgetObject->SetSpeed(Delta);
			InfoWidgetObject->SetDistance(Distance);
		}
		PreDistance = Distance;
	}
}

void APlanetActor::OnGravityZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
	{
		Cast<IMassActorInterface>(OtherActor)->PlanetOwner = this;
	}
}

void APlanetActor::OnGravityZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
	{
		Cast<IMassActorInterface>(OtherActor)->PlanetOwner = nullptr;
		IMassActorInterface::Execute_GravityActed(OtherActor, FVector::ZeroVector, 0.f);
	}
}
