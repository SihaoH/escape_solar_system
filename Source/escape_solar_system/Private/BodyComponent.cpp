// Copyright 2020 H₂S. All Rights Reserved.

#include "BodyComponent.h"
#include "MainFunctionLibrary.h"
#include <Kismet/KismetSystemLibrary.h>

#define IsChar(Type) (Type == EPawnType::MainChar)

UBodyComponent::UBodyComponent()
{
}

void UBodyComponent::SetStrength(EPawnType Type, int32 Level)
{
	MaximumHP = UMainFunctionLibrary::GetLevelValue(IsChar(Type) ? ELevel::CharHP : ELevel::ShipHP, Level);
	Mass = UMainFunctionLibrary::GetLevelValue(IsChar(Type) ? ELevel::CharMass : ELevel::ShipMass, Level);
}

void UBodyComponent::SetShieldCold(EPawnType Type, int32 Level)
{
	ShieldCold = UMainFunctionLibrary::GetLevelValue(IsChar(Type) ? ELevel::CharShieldCold : ELevel::ShipShieldCold, Level);
}

void UBodyComponent::SetShieldHeat(EPawnType Type, int32 Level)
{
	ShieldHeat = UMainFunctionLibrary::GetLevelValue(IsChar(Type) ? ELevel::CharShieldHeat : ELevel::ShipShieldHeat, Level);
}

void UBodyComponent::SetShieldPress(EPawnType Type, int32 Level)
{
	ShieldPress = UMainFunctionLibrary::GetLevelValue(IsChar(Type) ? ELevel::CharShieldPress : ELevel::ShipShieldPress, Level);
}

void UBodyComponent::ChangeHP(float Delta)
{
	CurrentHP += Delta;
	if (CurrentHP > MaximumHP)
	{
		CurrentHP = MaximumHP;
	}
	else if (CurrentHP <= 0)
	{
		CurrentHP = 0;
		// TODO 死亡
		UKismetSystemLibrary::PrintText(GetWorld(), INVTEXT("你死了！"));
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

	if (CurrentTemp > ShieldHeat)
	{
		ChangeHP(-(CurrentTemp - ShieldHeat));
		UMainFunctionLibrary::SendMessage(FText::Format(INVTEXT("机体过热，HP -{0}"), CurrentTemp - ShieldHeat));
	}
	else if (CurrentTemp < ShieldCold)
	{
		ChangeHP(-(ShieldCold - CurrentTemp));
		UMainFunctionLibrary::SendMessage(FText::Format(INVTEXT("机体过冷，HP -{0}"), ShieldCold - CurrentTemp));
	}

	if (CurrentTemp > ShieldPress)
	{
		ChangeHP(-(CurrentTemp - ShieldPress));
		UMainFunctionLibrary::SendMessage(FText::Format(INVTEXT("压力过大，HP -{0}"), CurrentTemp - ShieldPress));
	}
}

void UBodyComponent::OnComponentHitted(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const float HitSpeed = (HitComponent->GetComponentVelocity() - OtherActor->GetVelocity()).Size();
	if (HitSpeed > 1000)
	{
		ChangeHP(-HitSpeed / 100);
		UMainFunctionLibrary::SendMessage(FText::Format(INVTEXT("受到打击，HP -{0}"), HitSpeed / 100));
	}
}

