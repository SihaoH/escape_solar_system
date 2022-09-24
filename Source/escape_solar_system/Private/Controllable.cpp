// Copyright 2020 H₂S. All Rights Reserved.

#include "Controllable.h"
#include "CelestialBody.h"
#include "MassActorInterface.h"
#include "MainLibrary.h"
#include "GameFramework/Pawn.h"
#include <Kismet/GameplayStatics.h>

UControllable::UControllable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


class ACelestialBody* IControllable::LookedPlanet = nullptr;
class ACelestialBody* IControllable::LockedPlanet = nullptr;

float IControllable::GetGravityAccel() const
{
	return 0.0f;
}

void IControllable::GetLocationInfo(FText& Planet, FVector& Loction) const
{
	Planet = tr("太空");
	Loction = Cast<AActor>(this)->GetActorLocation();
	if (auto PlanetActor = Cast<IMassActorInterface>(this)->GetPlanetOwner())
	{
		Planet = PlanetActor->GetLabelName();
		Loction = PlanetActor->GetTransform().InverseTransformPosition(Loction);
	}
}

void IControllable::GetLocation(int& X, int& Y, int& Z)
{
	const FVector Location = Cast<AActor>(this)->GetActorLocation();
	X = Location.X / 100;
	Y = Location.Y / 100;
	Z = Location.Z / 100;
}

void IControllable::ChangePawn(APawn* NewPawn)
{
	auto Self = Cast<APawn>(this);
	auto PlayerController = UGameplayStatics::GetPlayerController(Self->GetWorld(), 0);
	PlayerController->bAutoManageActiveCameraTarget = false;
	if (NewPawn)
	{
		PlayerController->SetViewTargetWithBlend(NewPawn, 0.5f);
		if (NewPawn != PlayerController->GetPawn())
		{
			PlayerController->Possess(NewPawn);
		}
	}
	else
	{
		PlayerController->UnPossess();
	}

	if (NewPawn != Self) this->UnControlled();
	if (NewPawn) Cast<IControllable>(NewPawn)->Controlled();
}

void IControllable::LookPlanet()
{
	AActor* Self = Cast<AActor>(this);
	const APlayerController* Controller = UGameplayStatics::GetPlayerController(Self->GetWorld(), 0);
	if (Controller->GetPawn() == Self)
	{
		ACelestialBody* NewLookedPlanet = FindByLineTrace<ACelestialBody>(6400000.f);
		if (NewLookedPlanet != LookedPlanet)
		{
			if (LookedPlanet)
			{
				LookedPlanet->SetLooked(false);
			}
			if (NewLookedPlanet)
			{
				NewLookedPlanet->SetLooked(true);
			}
			LookedPlanet = NewLookedPlanet;
		}
	}
}

void IControllable::LockPlanet()
{
	if (LookedPlanet != nullptr)
	{
		if (LockedPlanet != LookedPlanet)
		{
			// 锁定注视的星球
			LookedPlanet->SetLocked(true);
			if (LockedPlanet)
			{
				LockedPlanet->SetLocked(false);
				LockedPlanet->SetLooked(false);
			}
			LockedPlanet = LookedPlanet;
		}
		else
		{
			// 解锁注视的星球
			LookedPlanet->SetLocked(false);
			LockedPlanet = nullptr;
		}
	}
}

void IControllable::ClearUp()
{
	LookedPlanet = nullptr;
	LockedPlanet = nullptr;
}

