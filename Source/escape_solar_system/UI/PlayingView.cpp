// Copyright 2020 H₂S. All Rights Reserved.

#include "PlayingView.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include <GameFramework/InputSettings.h>

UPlayingViewHelper::UPlayingViewHelper()
{
	CharOperDesc.Add("MoveForward", tr("向前或向后驱动"));
	CharOperDesc.Add("MoveRight", tr("左右驱动或(太空中)顺逆时针转动"));
	CharOperDesc.Add("MoveUp", tr("向上或向下驱动"));
	CharOperDesc.Add("Jump", tr("(陆地上)跳跃"));
	CharOperDesc.Add("Turn", tr("左右转动视角"));
	CharOperDesc.Add("LookUp", tr("上下转动视角"));
	CharOperDesc.Add("Lock", tr("锁定并显示星球信息"));
	CharOperDesc.Add("Drive", tr("登上飞船"));
	CharOperDesc.Add("Pickup", tr("拾取"));

	ShipOperDesc.Add("MoveForward", tr("向前或向后推进"));
	ShipOperDesc.Add("MoveRight", tr("向左或向右偏转机身"));
	ShipOperDesc.Add("MoveUp", tr("向上或向下推进"));
	ShipOperDesc.Add("Hold", tr("推进时按住自由转动视角"));
	ShipOperDesc.Add("Turn", tr("左右转动机身"));
	ShipOperDesc.Add("LookUp", tr("上下转动机身"));
	ShipOperDesc.Add("Lock", tr("锁定并显示星球信息"));
	ShipOperDesc.Add("Drive", tr("脱离飞船"));
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

FText UPlayingViewHelper::GetCharOperDesc(FName Name)
{
	return CharOperDesc[Name];
}

FText UPlayingViewHelper::GetShipOperDesc(FName Name)
{
	return ShipOperDesc[Name];
}
