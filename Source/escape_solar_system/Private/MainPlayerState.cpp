// Copyright 2020 H₂S. All Rights Reserved.


#include "MainPlayerState.h"

AMainPlayerState* AMainPlayerState::s_Instance = nullptr;

AMainPlayerState::AMainPlayerState()
{
	s_Instance = this;
}

void AMainPlayerState::UpdateBestLevel(const TArray<ELevel>& Props, int32 Val)
{
	int32* CurBest = GetBest(Props);
	if (Val > *CurBest)
	{
		*CurBest = Val;
	}
}

int32 AMainPlayerState::GetBestLevel(const TArray<ELevel>& Props)
{
	return *GetBest(Props);
}

void AMainPlayerState::ChangeExplorePoints(int32 Delta)
{
	ExplorePoints += Delta;
	ExplorePointsChangedDelegate.Broadcast(Delta);
	check(ExplorePoints > 0);
}

int32 AMainPlayerState::GetExplorePoints()
{
	return ExplorePoints;
}

int32* AMainPlayerState::GetBest(const TArray<ELevel>& Props)
{
	ELevel Level = Props[0];
	int32* Best = nullptr;
	switch (Level)
	{
	case ELevel::CharHP:
	case ELevel::CharMass:
		Best = &Best_CharBody;
		break;
	case ELevel::CharBackpack:
		Best = &Best_CharBackpack;
		break;
	case ELevel::CharShieldCold:
		Best = &Best_CharShieldCold;
		break;
	case ELevel::CharShieldHeat:
		Best = &Best_CharShieldHeat;
		break;
	case ELevel::CharShieldPress:
		Best = &Best_CharShieldPress;
		break;
	case ELevel::CharEnginePower:
	case ELevel::CharEngineMass:
	case ELevel::CharEngineEPR:
	case ELevel::CharEngineEMR:
		Best = &Best_CharEngine;
		break;
	case ELevel::CharEnergy:
		Best = &Best_CharEnergy;
		break;
	case ELevel::ShipHP:
	case ELevel::ShipMass:
		Best = &Best_ShipBody;
		break;
	case ELevel::ShipBackpack:
		Best = &Best_ShipBackpack;
		break;
	case ELevel::ShipShieldCold:
		Best = &Best_ShipShieldCold;
		break;
	case ELevel::ShipShieldHeat:
		Best = &Best_ShipShieldHeat;
		break;
	case ELevel::ShipShieldPress:
		Best = &Best_ShipShieldPress;
		break;
	case ELevel::ShipEngine0Power:
	case ELevel::ShipEngine0Mass:
	case ELevel::ShipEngine0EPR:
	case ELevel::ShipEngine0EMR:
		Best = &Best_ShipEngine[0];
		break;
	case ELevel::ShipEngine1Power:
	case ELevel::ShipEngine1Mass:
	case ELevel::ShipEngine1EPR:
	case ELevel::ShipEngine1EMR:
		Best = &Best_ShipEngine[1];
		break;
	case ELevel::ShipEngine2Power:
	case ELevel::ShipEngine2Mass:
	case ELevel::ShipEngine2EPR:
	case ELevel::ShipEngine2EMR:
		Best = &Best_ShipEngine[2];
		break;
	case ELevel::ShipEnergy0:
		Best = &Best_ShipEnergy[0];
		break;
	case ELevel::ShipEnergy1:
		Best = &Best_ShipEnergy[1];
		break;
	case ELevel::ShipEnergy2:
		Best = &Best_ShipEnergy[2];
		break;
	}

	return Best;
}
