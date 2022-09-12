// Copyright 2020 H₂S. All Rights Reserved.


#include "PlanetInfo.h"

void UPlanetInfo::SetLooked(bool bLooked)
{
	OnLooked(bLooked || bSelfLocked ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UPlanetInfo::SetLocked(bool bLocked)
{
	bSelfLocked = bLocked;
	OnLocked(bSelfLocked ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UPlanetInfo::SetName(FText Name, TObjectPtr<UTexture2D> Icon)
{
	OnNameSetted(Name, Icon);
}

void UPlanetInfo::SetDistance(float Dist)
{
	Dist /= 100; //转换为m
	FText DistText;
	if (Dist > 5000)
	{
		DistText = FText::Format(INVTEXT("{0}km"), FMath::RoundToInt(Dist / 1000));
	}
	else
	{
		DistText = FText::Format(INVTEXT("{0}m"), FMath::RoundToInt(Dist));
	}
	OnDistanceChanged(DistText);
}

void UPlanetInfo::SetSpeed(float Speed)
{
	Speed /= 100; //转换为m/s
	FText SpeedText;
	if (FMath::Abs(Speed) > 5000)
	{
		SpeedText = FText::Format(INVTEXT("{0}km/s"), FMath::RoundToInt(Speed / 1000));
	}
	else
	{
		SpeedText = FText::Format(INVTEXT("{0}m/s"), FMath::RoundToInt(Speed));
	}
	OnSpeedChanged(SpeedText);
}
