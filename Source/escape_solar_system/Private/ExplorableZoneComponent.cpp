// Copyright 2020 H₂S. All Rights Reserved.

#include "ExplorableZoneComponent.h"
#include "Controllable.h"
#include "MainPlayerState.h"
#include "MainLevelScript.h"
#define LOCTEXT_NAMESPACE "ExplorableZone"

UExplorableZoneComponent::UExplorableZoneComponent()
{
	OnComponentBeginOverlap.AddDynamic(this, &UExplorableZoneComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UExplorableZoneComponent::OnEndOverlap);

	Reset();
}

void UExplorableZoneComponent::Reset()
{
	SetHiddenInGame(!IsAvailable);
}

void UExplorableZoneComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsLoading())
	{
		SetHiddenInGame(!IsAvailable);
		SetCollisionEnabled(IsAvailable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

void UExplorableZoneComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IControllable>(OtherActor))
	{
		GetOwner()->GetWorldTimerManager().SetTimer(Timer, this, &UExplorableZoneComponent::HandleBonus, RequiredTime);
	}
}

void UExplorableZoneComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<IControllable>(OtherActor))
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(Timer);
	}
}

void UExplorableZoneComponent::HandleBonus()
{
	AMainPlayerState::Instance()->ChangeExplorePoints(BonusPoints);

	FText Msg = FText::Format(LOCTEXT("Msg", "到达{0}\n获得 {1} 探索点"), PlaceName, BonusPoints);
	AMainLevelScript::Instance()->ExplorePointsDelegate.Broadcast(Msg);

	IsAvailable = false;
	Reset();
}

#undef LOCTEXT_NAMESPACE
