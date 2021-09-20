﻿// Copyright 2020 H₂S. All Rights Reserved.

#include "PickableItemActor.h"
#include "MainFunctionLibrary.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Particles/ParticleSystemComponent.h>

APickableItemActor::APickableItemActor()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ScopeTigger = CreateDefaultSubobject<USphereComponent>(TEXT("ScopeTigger"));
	PromptEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PromptEffect"));

	RootComponent = StaticMesh;
	ScopeTigger->SetupAttachment(RootComponent);
	PromptEffect->SetupAttachment(RootComponent);
}

void APickableItemActor::Pickup(FName& RowName, int32& Count)
{
	FPickableItemData& Info = UMainFunctionLibrary::GetPickableItemData(ItemRowName);
	Deactivate(FMath::RandRange(Info.RebirthTime.Min, Info.RebirthTime.Max));
	RowName = ItemRowName;
	Count = FMath::RandRange(Info.PickCount.Min, Info.PickCount.Max);
}

void APickableItemActor::BeginPlay()
{
	Super::BeginPlay();
	ItemName = UMainFunctionLibrary::GetBasicItemData(ItemRowName).Name;
}

void APickableItemActor::Activate()
{
	PromptEffect->SetHiddenInGame(false);
	ScopeTigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APickableItemActor::Deactivate(int32 Time)
{
	PromptEffect->SetHiddenInGame(true);
	ScopeTigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(RebirthTimer, this, &APickableItemActor::Activate, Time, false);
}

