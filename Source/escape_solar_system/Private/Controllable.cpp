// Copyright 2020 H₂S. All Rights Reserved.

#include "Controllable.h"
#include "PlanetActor.h"
#include "GameFramework/Pawn.h"
#include <Kismet/GameplayStatics.h>

UControllable::UControllable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


class APlanetActor* IControllable::LookedPlanet = nullptr;
class APlanetActor* IControllable::LockedPlanet = nullptr;

void IControllable::GetHP(float& Current, float& Maximum) const
{
	Current = 1;
	Maximum = 1;
}

void IControllable::GetMP(float& Current, float& Maximum) const
{
	Current = 1;
	Maximum = 1;
}

float IControllable::GetGravityAccel() const
{
	return 0.0f;
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
	AActor* Self = Cast<AActor>(this);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Self->GetWorld(), 0);
	PlayerController->bAutoManageActiveCameraTarget = false;
	PlayerController->SetViewTargetWithBlend(NewPawn, 0.5f);
	PlayerController->Possess(NewPawn);

	this->UnControlled();
	Cast<IControllable>(NewPawn)->Controlled();
}

void IControllable::LookPlanet()
{
	AActor* Self = Cast<AActor>(this);
	const APlayerController* Controller = UGameplayStatics::GetPlayerController(Self->GetWorld(), 0);
	if (Controller->GetPawn() == Self)
	{
		const APlayerCameraManager* CameraManager = Controller->PlayerCameraManager;
		const FVector Start = CameraManager->GetCameraLocation();
		const FVector End = Start + CameraManager->GetTransformComponent()->GetForwardVector() * 6400000.f; //最长到64km
		FHitResult OutHit;
		APlanetActor* NewLookedPlanet = nullptr;
		if (UKismetSystemLibrary::LineTraceSingle(
			Self->GetWorld(), Start, End,
			ETraceTypeQuery::TraceTypeQuery1, true, { Self }, EDrawDebugTrace::ForOneFrame, OutHit, true))
		{
			NewLookedPlanet = Cast<APlanetActor>(OutHit.Actor);
		}
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

