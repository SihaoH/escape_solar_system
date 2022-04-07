// Copyright 2020 H₂S. All Rights Reserved.

#include "EngineComponent.h"
#include "Controllable.h"

UEngineComponent::UEngineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEngineComponent::SetEngine(EPawnType PType, int32 Level, int32 Type)
{
	ELevel PowerLV = ELevel::CharEnginePower;
	ELevel MassLV  = ELevel::CharEngineMass;
	ELevel EprLV   = ELevel::CharEngineEPR;
	ELevel EmrLV   = ELevel::CharEngineEMR;
	if (PType == EPawnType::SpaceShip)
	{
		switch (Type)
		{
		case 0:
			PowerLV = ELevel::ShipEngine0Power;
			MassLV  = ELevel::ShipEngine0Mass;
			EprLV   = ELevel::ShipEngine0EPR;
			EmrLV   = ELevel::ShipEngine0EMR;
			break;
		case 1:
			PowerLV = ELevel::ShipEngine1Power;
			MassLV  = ELevel::ShipEngine1Mass;
			EprLV   = ELevel::ShipEngine1EPR;
			EmrLV   = ELevel::ShipEngine1EMR;
			break;
		case 2:
			PowerLV = ELevel::ShipEngine1Power;
			MassLV  = ELevel::ShipEngine1Mass;
			EprLV   = ELevel::ShipEngine1EPR;
			EmrLV   = ELevel::ShipEngine1EMR;
			break;
		}
	}
	Power = UMainFunctionLibrary::GetLevelValue(PowerLV, Level);
	Mass = UMainFunctionLibrary::GetLevelValue(MassLV, Level);
	EPRatio = UMainFunctionLibrary::GetLevelValue(EprLV, Level);
	EMRatio = UMainFunctionLibrary::GetLevelValue(EmrLV, Level);
}

void UEngineComponent::SetEnergy(EPawnType PType, int32 Level, int32 Type)
{
	ELevel LV = ELevel::CharEnergy;
	if (PType == EPawnType::SpaceShip)
	{
		switch (Type)
		{
		case 0:
			LV = ELevel::ShipEnergy0;
			break;
		case 1:
			LV = ELevel::ShipEnergy1;
			break;
		case 2:
			LV = ELevel::ShipEnergy1;
			break;
		}
	}
	MaximumEnergy = UMainFunctionLibrary::GetLevelValue(LV, Level);
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

void UEngineComponent::UseEnergy(float Used)
{
	ChangeEnergy(-Used / EPRatio * GetWorld()->GetDeltaSeconds());
}
