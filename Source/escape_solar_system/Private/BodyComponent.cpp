﻿// Copyright 2020 H₂S. All Rights Reserved.

#include "BodyComponent.h"
#include "CelestialBody.h"
#include "MassActorInterface.h"
#include "FluidZoneComponent.h"
#include "Controllable.h"
#include "MainLibrary.h"
#include <Kismet/KismetSystemLibrary.h>

#define IsChar(Type) (Type == EPawnType::MainChar)

UBodyComponent::UBodyComponent()
{
}

void UBodyComponent::SetStrength(EPawnType Type, int32 Level)
{
	MaximumHP = UMainLibrary::GetTechValue(IsChar(Type) ? ETech::CharHP : ETech::ShipHP, Level);
	Mass = UMainLibrary::GetTechValue(IsChar(Type) ? ETech::CharMass : ETech::ShipMass, Level);
}

void UBodyComponent::SetShieldCold(EPawnType Type, int32 Level)
{
	ShieldCold = UMainLibrary::GetTechValue(IsChar(Type) ? ETech::CharShieldCold : ETech::ShipShieldCold, Level);
}

void UBodyComponent::SetShieldHeat(EPawnType Type, int32 Level)
{
	ShieldHeat = UMainLibrary::GetTechValue(IsChar(Type) ? ETech::CharShieldHeat : ETech::ShipShieldHeat, Level);
}

void UBodyComponent::SetShieldPress(EPawnType Type, int32 Level)
{
	ShieldPress = UMainLibrary::GetTechValue(IsChar(Type) ? ETech::CharShieldPress : ETech::ShipShieldPress, Level);
}

void UBodyComponent::ChangeHP(int32 Delta)
{
	CurrentHP += Delta;
	if (CurrentHP > MaximumHP)
	{
		CurrentHP = MaximumHP;
	}
	else if (CurrentHP <= 0)
	{
		CurrentHP = 0;
	}
	HpChangedDelegate.Broadcast(Delta);
}

void UBodyComponent::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(EnvTimer, this, &UBodyComponent::CheckEnvironment, 1.f, true, 0.f);
}

void UBodyComponent::CheckEnvironment()
{
	const auto FluidZone = Cast<IMassActorInterface>(GetOwner())->GetFluidZone();
	CurrentTemp = FluidZone ? FluidZone->HeatTransfer(CurrentTemp) : CurrentTemp;
	CurrentPress = FluidZone ? FluidZone->PressTransfer(GetOwner()->GetActorLocation()) : 0;

	const FText OwnerName = Cast<IControllable>(GetOwner())->GetLabelName();

	int32 Delta = 0;
	if (CurrentTemp > ShieldHeat)
	{
		Delta = -(CurrentTemp - ShieldHeat);
		ChangeHP(Delta);
		UMainLibrary::SendMessage(FText::Format(tr("{0}过热，HP {1}"), OwnerName, Delta));
	}
	else if (CurrentTemp < ShieldCold)
	{
		Delta = -(ShieldCold - CurrentTemp);
		ChangeHP(Delta);
		UMainLibrary::SendMessage(FText::Format(tr("{0}过冷，HP {1}"), OwnerName, Delta));
	}

	if (CurrentPress > ShieldPress)
	{
		Delta = -(CurrentPress - ShieldPress);
		ChangeHP(-(CurrentPress - ShieldPress));
		UMainLibrary::SendMessage(FText::Format(tr("{0}过压，HP {1}"), OwnerName, Delta));
	}
}

void UBodyComponent::OnComponentHitted(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FVector OtherVelocity = OtherActor->GetVelocity();
	if (auto CelestialBody = Cast<ACelestialBody>(OtherActor))
	{
		OtherVelocity = CelestialBody->GetStaticMeshComponent()->GetPhysicsLinearVelocityAtPoint(GetOwner()->GetActorLocation());
	}
	constexpr float Threshold = 1000.f;
	const float HitSpeed = (HitComponent->GetComponentVelocity() - OtherVelocity).Size() - Threshold;
	if (HitSpeed > 0)
	{
		int32 Delta = -HitSpeed / 100;
		ChangeHP(Delta);
		const FText OwnerName = Cast<IControllable>(GetOwner())->GetLabelName();
		UMainLibrary::SendMessage(FText::Format(tr("{0}受击，HP {1}"), OwnerName, Delta));
	}
}

