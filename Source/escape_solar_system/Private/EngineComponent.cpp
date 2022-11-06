// Copyright 2020 H₂S. All Rights Reserved.

#include "EngineComponent.h"
#include "Controllable.h"

UEngineComponent::UEngineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEngineComponent::SetEngine(EPawnType PType, int32 Level, int32 Type)
{
	ETech PowerLV = ETech::CharEnginePower;
	ETech MassLV  = ETech::CharEngineMass;
	ETech EprLV   = ETech::CharEngineEPR;
	ETech EmrLV   = ETech::CharEngineEMR;
	if (PType == EPawnType::SpaceShip)
	{
		switch (Type)
		{
		case 0:
			PowerLV = ETech::ShipEngine0Power;
			MassLV  = ETech::ShipEngine0Mass;
			EprLV   = ETech::ShipEngine0EPR;
			EmrLV   = ETech::ShipEngine0EMR;
			break;
		case 1:
			PowerLV = ETech::ShipEngine1Power;
			MassLV  = ETech::ShipEngine1Mass;
			EprLV   = ETech::ShipEngine1EPR;
			EmrLV   = ETech::ShipEngine1EMR;
			break;
		case 2:
			PowerLV = ETech::ShipEngine1Power;
			MassLV  = ETech::ShipEngine1Mass;
			EprLV   = ETech::ShipEngine1EPR;
			EmrLV   = ETech::ShipEngine1EMR;
			break;
		}
	}
	Power = UMainLibrary::GetTechValueVal(PowerLV, Level);
	Mass = UMainLibrary::GetTechValueVal(MassLV, Level);
	EPRatio = UMainLibrary::GetTechValueVal(EprLV, Level);
	EMRatio = UMainLibrary::GetTechValueVal(EmrLV, Level);
}

void UEngineComponent::SetEnergy(EPawnType PType, int32 Level, int32 Type)
{
	ETech LV = ETech::CharEnergy;
	if (PType == EPawnType::SpaceShip)
	{
		switch (Type)
		{
		case 0:
			LV = ETech::ShipEnergy0;
			break;
		case 1:
			LV = ETech::ShipEnergy1;
			break;
		case 2:
			LV = ETech::ShipEnergy1;
			break;
		}
	}
	MaximumEnergy = UMainLibrary::GetTechValueVal(LV, Level);
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
	EnergyChangedDelegate.Broadcast(Delta);
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

void UEngineComponent::UseEnergy(float Used)
{
	ChangeEnergy(-Used / EPRatio * GetWorld()->GetDeltaSeconds());
}
