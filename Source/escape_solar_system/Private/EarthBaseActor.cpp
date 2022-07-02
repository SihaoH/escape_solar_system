// Copyright 2020 H₂S. All Rights Reserved.

#include "EarthBaseActor.h"
#include "MainLevelScriptActor.h"
#include "BackpackComponent.h"
#include "Spaceship.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>

AEarthBase::AEarthBase()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ScopeTigger = CreateDefaultSubobject<USphereComponent>(TEXT("ScopeTigger"));
	Backpack = CreateDefaultSubobject<UBackpackComponent>(TEXT("Backpack"));

	RootComponent = StaticMesh;
	ScopeTigger->SetupAttachment(GetRootComponent());
	ScopeTigger->SetSphereRadius(1000.f);

	// FClassFinder只能在构造函数中调用
	BP_SpaceshipClass = ConstructorHelpers::FClassFinder<ASpaceship>(TEXT("Blueprint'/Game/MainBP/Blueprints/BP_Spaceship.BP_Spaceship_C'")).Class;
}

void AEarthBase::CreateSpaceship()
{
	check(!AMainLevelScriptActor::GetSpaceship());
	GetWorld()->SpawnActor<ASpaceship>(BP_SpaceshipClass, GetActorLocation(), GetActorRotation());
}

ASpaceship* AEarthBase::FindSpaceship() const
{
	TArray<AActor*> NearbyActors;
	ScopeTigger->GetOverlappingActors(NearbyActors, ASpaceship::StaticClass());
	if (NearbyActors.Num() > 0)
	{
		return Cast<ASpaceship>(NearbyActors[0]);
	}
	return nullptr;
}

void AEarthBase::BeginPlay()
{
	Super::BeginPlay();
}
