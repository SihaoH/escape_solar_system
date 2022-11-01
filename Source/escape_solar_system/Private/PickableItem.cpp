// Copyright 2020 H₂S. All Rights Reserved.

#include "PickableItem.h"
#include "MainLibrary.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Particles/ParticleSystemComponent.h>

UPickableItem::UPickableItem()
{
	PromptEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PromptEffect"));
	PromptEffect->SetupAttachment(this);

	Enable();
}

void UPickableItem::Pickup(FName& RowName, int32& Count)
{
	FItemData& ItemData = UMainLibrary::GetItemData(ItemRowName);
	Disable(FMath::RandRange(ItemData.RebirthTime.Min, ItemData.RebirthTime.Max));
	RowName = ItemRowName;
	Count = FMath::RandRange(ItemData.PickCount.Min, ItemData.PickCount.Max);
}

void UPickableItem::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsLoading() && GWorld)
	{
		Disable(Countdown);
	}
}

void UPickableItem::BeginPlay()
{
	Super::BeginPlay();
	ItemName = UMainLibrary::GetItemData(ItemRowName).Name;
}

void UPickableItem::Enable()
{
	PromptEffect->SetHiddenInGame(false);
	SetCollisionProfileName("PickableItem_Enable");
}

void UPickableItem::Disable(int32 Time)
{
	if (Time <= 0) return;
	PromptEffect->SetHiddenInGame(true);
	SetCollisionProfileName("PickableItem_Disable");

	Countdown = Time;
	GetOwner()->GetWorldTimerManager().SetTimer(RebirthTimer, this, &UPickableItem::OnCountdown, 1.0f, true);
}

void UPickableItem::OnCountdown()
{
	Countdown -= 1;
	if (Countdown <= 0)
	{
		Countdown = 0;
		GetOwner()->GetWorldTimerManager().ClearTimer(RebirthTimer);
		RebirthTimer.Invalidate();
		Enable();
	}
}

