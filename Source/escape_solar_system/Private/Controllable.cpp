// Copyright 2020 H₂S. All Rights Reserved.

#include "Controllable.h"
#include "CelestialBody.h"
#include "MassActorInterface.h"
#include "MainLibrary.h"
#include "MainLevelScript.h"
#include "GameFramework/Pawn.h"
#include <Kismet/GameplayStatics.h>

UControllable::UControllable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


class ACelestialBody* IControllable::LookedCelestialBody = nullptr;
class ACelestialBody* IControllable::LockedCelestialBody = nullptr;

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
		ACelestialBody* Body = FindByLineTrace<ACelestialBody>(6400000.f);
		if (Body != LookedCelestialBody)
		{
			LookedCelestialBody = Body;
			AMainLevelScript::LookCelestialBody(LookedCelestialBody);
		}
	}
}

void IControllable::LockPlanet()
{
	if (LookedCelestialBody != nullptr)
	{
		if (LockedCelestialBody != LookedCelestialBody)
		{
			// 锁定注视的星球
			LockedCelestialBody = LookedCelestialBody;
		}
		else
		{
			// 解锁注视的星球
			LockedCelestialBody = nullptr;
		}
		AMainLevelScript::LockCelestialBody(LockedCelestialBody);
	}
}

void IControllable::ClearUp()
{
	LookedCelestialBody = nullptr;
	LockedCelestialBody = nullptr;
}

