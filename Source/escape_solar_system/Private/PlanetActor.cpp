// Copyright 2020 H₂S. All Rights Reserved.


#include "PlanetActor.h"
#include "MassActorInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

APlanetActor::APlanetActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	GravityZone = CreateDefaultSubobject<USphereComponent>(TEXT("GravityZone"));
	GravityZone->SetupAttachment(GetRootComponent());
	GravityZone->OnComponentEndOverlap.AddDynamic(this, &APlanetActor::OnComponentEndOverlap);

	InfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidget"));
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
	CallWidgetFunc("SetLooked", &Looked);
}

void APlanetActor::SetLocked(bool Locked)
{
	CallWidgetFunc("SetLocked", &Locked);
}

void APlanetActor::BeginPlay()
{
	Super::BeginPlay();
	SelfRadius = GetStaticMeshComponent()->Bounds.SphereRadius;
	if (CentralActor)
	{
		DistanceRadius = FVector::Distance(CentralActor->GetActorLocation(), GetActorLocation());
	}

	CallWidgetFunc("SetNameText", &Name);
	GetWorldTimerManager().SetTimer(InfoTimer, this, &APlanetActor::UpdateInfoWidget, 0.1f, true, 0.f);
}

void APlanetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformRotation(DeltaTime);
	PerformRevolution(DeltaTime);
	PerformGravity(DeltaTime);
}

void APlanetActor::CallWidgetFunc(const FName& FuncName, void* FuncParm)
{
	UUserWidget* InfoWidgetObj = InfoWidget->GetUserWidgetObject();
	if (InfoWidgetObj)
	{
		UFunction* Func = InfoWidgetObj->GetClass()->FindFunctionByName(FuncName);
		check(Func);
		InfoWidgetObj->ProcessEvent(Func, FuncParm);
	}
}

void APlanetActor::PerformRotation(float DeltaTime)
{
	if (RotationSpeed != 0)
	{
		AddActorLocalRotation(FRotator(0, RotationSpeed * DeltaTime, 0));
	}
}

/**
 * 计算坐标，处理公转动作
 * 因为精度问题，在接近0度时，计算出来的当前位置可能和上个位置一样，导致星球不动
 * 所以在此加了个循环积累值，当前帧的位置必须较上一帧有所变化
 */
void APlanetActor::PerformRevolution(float DeltaTime)
{
	if (CentralActor && RevolutionSpeed != 0)
	{
		FVector CentralLocation = CentralActor->GetActorLocation();
		FVector Location = GetActorLocation();
		Location = Location - CentralLocation;
		float Angle = FMath::Acos(Location.CosineAngle2D(FVector(1.f, 0, 0))) * FMath::Sign(Location.Y);
		float NewX = Location.X;
		float NewY = Location.Y;
		do {
			Angle += FMath::DegreesToRadians(RevolutionSpeed * DeltaTime);
			NewX = FMath::Cos(Angle) * DistanceRadius;
			NewY = FMath::Sin(Angle) * DistanceRadius;
		} while (FMath::IsNearlyEqual(NewX, Location.X, 1.f) && FMath::IsNearlyEqual(NewY, Location.Y, 1.f));
		SetActorLocation(FVector(NewX, NewY, 0) + CentralLocation);
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
			const FVector SelfLocation = GetActorLocation();
			const FVector TargetLocation = Actor->GetActorLocation();
			FVector Direction = (SelfLocation - TargetLocation).GetSafeNormal();
			float Distance = FVector::Dist(SelfLocation, TargetLocation) - SelfRadius;
			float Accel = SurfaceGravity * FMath::Max((1 - Distance / 3 / SelfRadius), 0.f);

			Cast<IMassActorInterface>(Actor)->PlanetOwner = this;
			IMassActorInterface::Execute_GravityActed(Actor, Direction, Accel);
		}
	}
}

void APlanetActor::UpdateInfoWidget()
{
	const APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	float Distance = FVector::Dist(Player->GetActorLocation(), GetActorLocation()) - SelfRadius;
	if (PreDistance != 0)
	{
		float Delta = (Distance - PreDistance) / 0.1;
		CallWidgetFunc("SetSpeedValue", &Delta);
		CallWidgetFunc("SetDistanceValue", &Distance);
	}
	PreDistance = Distance;
}

void APlanetActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
	{
		Cast<IMassActorInterface>(OtherActor)->PlanetOwner = nullptr;
		IMassActorInterface::Execute_GravityActed(OtherActor, FVector::ZeroVector, 0.f);
	}
}
