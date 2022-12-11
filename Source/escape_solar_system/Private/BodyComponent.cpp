// Copyright 2020 H₂S. All Rights Reserved.

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
	MaximumHP = UMainLibrary::GetTechValueVal(IsChar(Type) ? ETech::CharHP : ETech::ShipHP, Level);
	Mass = UMainLibrary::GetTechValueVal(IsChar(Type) ? ETech::CharMass : ETech::ShipMass, Level);
}

void UBodyComponent::SetShieldCold(EPawnType Type, int32 Level)
{
	ShieldCold = UMainLibrary::GetTechValueVal(IsChar(Type) ? ETech::CharShieldCold : ETech::ShipShieldCold, Level);
}

void UBodyComponent::SetShieldHeat(EPawnType Type, int32 Level)
{
	ShieldHeat = UMainLibrary::GetTechValueVal(IsChar(Type) ? ETech::CharShieldHeat : ETech::ShipShieldHeat, Level);
}

void UBodyComponent::SetShieldPress(EPawnType Type, int32 Level)
{
	ShieldPress = UMainLibrary::GetTechValueVal(IsChar(Type) ? ETech::CharShieldPress : ETech::ShipShieldPress, Level);
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
	// 1秒的伤害间隔，防止连续伤害
	const static FTimespan CD = FTimespan(0, 0, 1);
	if (FDateTime::Now() - HittedTime < CD)
	{
		return;
	}
	HittedTime = FDateTime::Now();

	FVector OtherVelocity = OtherActor->GetVelocity();
	if (GetOwner()->GetAttachParentActor() == OtherActor)
	{
		OtherVelocity = FVector::ZeroVector;
	}
	constexpr float Threshold = 1000.f;
	const float HitSpeed = (HitComponent->GetComponentVelocity() - OtherVelocity).Size() - Threshold;

	// 伤害值，每100cm/s扣10点血
	constexpr float DamageVal = 100.f / 10.f;
	if (HitSpeed / DamageVal > 0)
	{
		int32 Delta = -HitSpeed / DamageVal;
		ChangeHP(Delta);
		const FText OwnerName = Cast<IControllable>(GetOwner())->GetLabelName();
		UMainLibrary::SendMessage(FText::Format(tr("{0}受击，HP {1}"), OwnerName, Delta));
	}
}

