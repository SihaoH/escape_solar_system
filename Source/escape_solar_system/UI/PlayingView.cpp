// Copyright 2020 H₂S. All Rights Reserved.

#include "PlayingView.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include <GameFramework/InputSettings.h>

UPlayingViewHelper::UPlayingViewHelper()
{
}

TArray<FKey> UPlayingViewHelper::GetActionKeys(FName ActionName)
{
	TArray<FInputActionKeyMapping> Mappings;
	TArray<FKey> Keys;
	auto InputSettings = UInputSettings::GetInputSettings();
	InputSettings->GetActionMappingByName(ActionName, Mappings);
	for (const auto& KeyMap : Mappings)
	{
		Keys.Add(KeyMap.Key);
	}
	return Keys;
}

TArray<FKey> UPlayingViewHelper::GetAxisKeys(FName AxisName)
{
	TArray<FInputAxisKeyMapping> Mappings;
	TArray<FKey> Keys;
	auto InputSettings = UInputSettings::GetInputSettings();
	InputSettings->GetAxisMappingByName(AxisName, Mappings);
	for (const auto& KeyMap : Mappings)
	{
		Keys.Add(KeyMap.Key);
	}
	return Keys;
}
