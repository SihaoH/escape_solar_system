// Copyright 2020 H₂S. All Rights Reserved.

#include "PickableItemActor.h"
#include "MainLibrary.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Particles/ParticleSystemComponent.h>

APickableItemActor::APickableItemActor()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ScopeTigger = CreateDefaultSubobject<USphereComponent>(TEXT("ScopeTigger"));
	PromptEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PromptEffect"));
	StaticMesh->SetCollisionProfileName("InvisibleWallDynamic");
	ScopeTigger->SetCollisionProfileName("UI");

	RootComponent = StaticMesh;
	ScopeTigger->SetupAttachment(RootComponent);
	PromptEffect->SetupAttachment(RootComponent);
}

void APickableItemActor::SetHighlight(bool bValue)
{
	StaticMesh->SetRenderCustomDepth(bValue);
}

void APickableItemActor::Pickup(FName& RowName, int32& Count)
{
	FItemData& ItemData = UMainLibrary::GetItemData(ItemRowName);
	Deactivate(FMath::RandRange(ItemData.RebirthTime.Min, ItemData.RebirthTime.Max));
	RowName = ItemRowName;
	Count = FMath::RandRange(ItemData.PickCount.Min, ItemData.PickCount.Max);
}

void APickableItemActor::BeginPlay()
{
	Super::BeginPlay();
	ItemName = UMainLibrary::GetItemData(ItemRowName).Name;
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

