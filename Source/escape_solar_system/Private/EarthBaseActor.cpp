// Copyright 2020 H₂S. All Rights Reserved.

#include "EarthBaseActor.h"
#include "BackpackComponent.h"
#include "Spaceship.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>

AEarthBaseActor::AEarthBaseActor()
{
	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	TiggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TiggerComponent"));
	Storehouse = CreateDefaultSubobject<UBackpackComponent>(TEXT("Backpack"));

	TiggerComponent->SetupAttachment(GetRootComponent());
	TiggerComponent->SetSphereRadius(1000.f);
	Storehouse->MaxGrid = 50;
	Storehouse->MaxBearing = -1.f;
}

ASpaceship* AEarthBaseActor::FindSpaceship() const
{
	TArray<AActor*> NearbyActors;
	TiggerComponent->GetOverlappingActors(NearbyActors, ASpaceship::StaticClass());
	if (NearbyActors.Num() > 0)
	{
		return Cast<ASpaceship>(NearbyActors[0]);
	}
	return nullptr;
}

void AEarthBaseActor::BeginPlay()
{
	Super::BeginPlay();

	Storehouse->AddItem("0000", 44);
	Storehouse->AddItem("1000", 66);
}
