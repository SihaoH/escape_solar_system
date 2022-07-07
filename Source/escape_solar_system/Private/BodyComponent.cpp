// Copyright 2020 H₂S. All Rights Reserved.

#include "BodyComponent.h"
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
	// TODO 外界温度影响区域
	CurrentTemp = 0;

	// TODO 外界压力影响区域
	CurrentPress = 0;

	int32 Delta = 0;
	if (CurrentTemp > ShieldHeat)
	{
		Delta = -(CurrentTemp - ShieldHeat);
		ChangeHP(Delta);
		UMainLibrary::SendMessage(FText::Format(INVTEXT("机体过热，HP {0}"), Delta));
	}
	else if (CurrentTemp < ShieldCold)
	{
		Delta = -(ShieldCold - CurrentTemp);
		ChangeHP(Delta);
		UMainLibrary::SendMessage(FText::Format(INVTEXT("机体过冷，HP {0}"), Delta));
	}

	if (CurrentTemp > ShieldPress)
	{
		Delta = -(CurrentTemp - ShieldPress);
		ChangeHP(-(CurrentTemp - ShieldPress));
		UMainLibrary::SendMessage(FText::Format(INVTEXT("压力过大，HP {0}"), Delta));
	}
}

void UBodyComponent::OnComponentHitted(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const float HitSpeed = (HitComponent->GetComponentVelocity() - OtherActor->GetVelocity()).Size();
	if (HitSpeed > 1000)
	{
		int32 Delta = -HitSpeed / 100;
		ChangeHP(Delta);
		UMainLibrary::SendMessage(FText::Format(INVTEXT("受到打击，HP {0}"), Delta));
	}
}

