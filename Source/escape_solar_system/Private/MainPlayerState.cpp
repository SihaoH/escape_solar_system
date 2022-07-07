// Copyright 2020 H₂S. All Rights Reserved.


#include "MainPlayerState.h"

AMainPlayerState* AMainPlayerState::s_Instance = nullptr;

AMainPlayerState::AMainPlayerState()
{
	s_Instance = this;
}

void AMainPlayerState::UpdateBestLevel(const TArray<ETech>& Props, int32 Val)
{
	int32* CurBest = GetBest(Props);
	if (Val > *CurBest)
	{
		*CurBest = Val;
	}
}

int32 AMainPlayerState::GetBestLevel(const TArray<ETech>& Props)
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

int32* AMainPlayerState::GetBest(const TArray<ETech>& Props)
{
	ETech Level = Props[0];
	int32* Best = nullptr;
	switch (Level)
	{
	case ETech::CharHP:
	case ETech::CharMass:
		Best = &Best_CharBody;
		break;
	case ETech::CharBackpack:
		Best = &Best_CharBackpack;
		break;
	case ETech::CharShieldCold:
		Best = &Best_CharShieldCold;
		break;
	case ETech::CharShieldHeat:
		Best = &Best_CharShieldHeat;
		break;
	case ETech::CharShieldPress:
		Best = &Best_CharShieldPress;
		break;
	case ETech::CharEnginePower:
	case ETech::CharEngineMass:
	case ETech::CharEngineEPR:
	case ETech::CharEngineEMR:
		Best = &Best_CharEngine;
		break;
	case ETech::CharEnergy:
		Best = &Best_CharEnergy;
		break;
	case ETech::ShipHP:
	case ETech::ShipMass:
		Best = &Best_ShipBody;
		break;
	case ETech::ShipBackpack:
		Best = &Best_ShipBackpack;
		break;
	case ETech::ShipShieldCold:
		Best = &Best_ShipShieldCold;
		break;
	case ETech::ShipShieldHeat:
		Best = &Best_ShipShieldHeat;
		break;
	case ETech::ShipShieldPress:
		Best = &Best_ShipShieldPress;
		break;
	case ETech::ShipEngine0Power:
	case ETech::ShipEngine0Mass:
	case ETech::ShipEngine0EPR:
	case ETech::ShipEngine0EMR:
		Best = &Best_ShipEngine[0];
		break;
	case ETech::ShipEngine1Power:
	case ETech::ShipEngine1Mass:
	case ETech::ShipEngine1EPR:
	case ETech::ShipEngine1EMR:
		Best = &Best_ShipEngine[1];
		break;
	case ETech::ShipEngine2Power:
	case ETech::ShipEngine2Mass:
	case ETech::ShipEngine2EPR:
	case ETech::ShipEngine2EMR:
		Best = &Best_ShipEngine[2];
		break;
	case ETech::ShipEnergy0:
		Best = &Best_ShipEnergy[0];
		break;
	case ETech::ShipEnergy1:
		Best = &Best_ShipEnergy[1];
		break;
	case ETech::ShipEnergy2:
		Best = &Best_ShipEnergy[2];
		break;
	}

	return Best;
}
