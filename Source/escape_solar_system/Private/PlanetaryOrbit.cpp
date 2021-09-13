// Copyright 2020 H₂S. All Rights Reserved.

#include "PlanetaryOrbit.h"

APlanetaryOrbit::APlanetaryOrbit()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void APlanetaryOrbit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CentralActor && RevolutionSpeed != 0)
	{
		SetActorLocation(CentralActor->GetActorLocation());
		GetRootComponent()->AddLocalRotation(FRotator(0.f, RevolutionSpeed * DeltaTime, 0.f));
	}
}

