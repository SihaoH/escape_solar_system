// Copyright 2020 H₂S. All Rights Reserved.

#include "EngineComponent.h"
#include "Controllable.h"

UEngineComponent::UEngineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEngineComponent::ChangeEnergy(float Delta)
{
	CurrentEnergy += Delta;
	if (CurrentEnergy > MaximumEnergy)
	{
		CurrentEnergy = MaximumEnergy;
	}
	else if (CurrentEnergy <= 0)
	{
		CurrentEnergy = 0;
	}
}

void UEngineComponent::MoveForward(float Value)
{
	if (Value == 0 || CurrentEnergy <= 0)
	{
		ForwardForce = 0;
		return;
	}
	if (FMath::Sign(ForwardForce) != FMath::Sign(Value))
	{
		ForwardForce = 0;
	}
	ForwardForce += Power * GetWorld()->GetDeltaSeconds() * Value;
	if (FMath::Abs(ForwardForce) > FMath::Abs(Power * Value))
	{
		ForwardForce = Power * Value;
	}
	UseEnergy(FMath::Abs(ForwardForce));
	Cast<IControllable>(GetOwner())->Thrusting(GetOwner()->GetRootComponent()->GetForwardVector() * ForwardForce * 100);
}

void UEngineComponent::MoveUp(float Value)
{
	if (Value == 0 || CurrentEnergy <= 0)
	{
		UpForce = 0;
		return;
	}
	if (FMath::Sign(UpForce) != FMath::Sign(Value))
	{
		UpForce = 0;
	}
	UpForce += Power * GetWorld()->GetDeltaSeconds() * Value;
	if (FMath::Abs(UpForce) > FMath::Abs(Power * Value))
	{
		UpForce = Power * Value;
	}
	UseEnergy(FMath::Abs(UpForce));
	Cast<IControllable>(GetOwner())->Thrusting(GetOwner()->GetRootComponent()->GetUpVector() * UpForce * 100);
}

void UEngineComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEngineComponent::UseEnergy(float Used)
{
	ChangeEnergy(-Used / EPRatio * GetWorld()->GetDeltaSeconds());
}
