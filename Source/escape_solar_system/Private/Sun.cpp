// Copyright 2020 H₂S. All Rights Reserved.

#include "Sun.h"
#include "MassActorInterface.h"
#include "Controllable.h"
#include "FluidZoneComponent.h"
#include <EngineUtils.h>

ASun::ASun()
{
	Type = ECelestialBodyType::Star;

	InnerZone = CreateDefaultSubobject<UFluidZoneComponent>(TEXT("InnerZone"));
	InnerZone->SetupAttachment(GetRootComponent());
}

void ASun::PerformRotation(float DeltaTime)
{
	// PASS 不自转
}

void ASun::PerformGravity(float DeltaTime)
{
	for (auto It = TActorIterator<AActor>(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()) && !Cast<IMassActorInterface>(Actor)->GetPlanetOwner())
		{
			FVector Direction = FVector::ZeroVector;
			float Accel = 0;
			CalcGravityResult(Actor, Direction, Accel);
			IMassActorInterface::Execute_GravityActedGlobally(Actor, Direction, Accel);
		}
	}
}

/** 使用GravityZone作为内部区域，除了可控制的角色，被太阳吞噬的物品都被销毁 */
void ASun::OnGravityZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && !Cast<IControllable>(OtherActor))
	{
		OtherActor->Destroy();
	}
}

void ASun::OnGravityZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// PASS 不处理离开事件
}
