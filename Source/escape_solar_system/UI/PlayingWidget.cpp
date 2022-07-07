// Copyright 2020 H₂S. All Rights Reserved.

#include "PlayingWidget.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"
#include <GameFramework/InputSettings.h>
#define LOCTEXT_NAMESPACE "PlayView"

UPlayingHelper::UPlayingHelper()
{
	CharOperDesc.Add("MoveForward", LOCTEXT("CharMoveForward", "向前或向后驱动"));
	CharOperDesc.Add("MoveRight", LOCTEXT("CharMoveRight", "左右驱动或(太空中)顺逆时针转动"));
	CharOperDesc.Add("MoveUp", LOCTEXT("CharMoveUp", "向上或向下驱动"));
	CharOperDesc.Add("Jump", LOCTEXT("CharJump", "(陆地上)跳跃"));
	CharOperDesc.Add("Turn", LOCTEXT("CharTurn", "左右转动视角"));
	CharOperDesc.Add("LookUp", LOCTEXT("CharLookUp", "上下转动视角"));
	CharOperDesc.Add("Lock", LOCTEXT("CharLock", "锁定并显示星球信息"));
	CharOperDesc.Add("Drive", LOCTEXT("CharDrive", "登上飞船"));
	CharOperDesc.Add("Pickup", LOCTEXT("CharPickup", "拾取"));

	ShipOperDesc.Add("MoveForward", LOCTEXT("ShipMoveForward", "向前或向后推进"));
	ShipOperDesc.Add("MoveRight", LOCTEXT("ShipMoveRight", "向左或向右偏转机身"));
	ShipOperDesc.Add("MoveUp", LOCTEXT("ShipMoveUp", "向上或向下推进"));
	ShipOperDesc.Add("Hold", LOCTEXT("ShipHold", "推进时按住自由转动视角"));
	ShipOperDesc.Add("Turn", LOCTEXT("ShipTurn", "左右转动机身"));
	ShipOperDesc.Add("LookUp", LOCTEXT("ShipLookUp", "上下转动机身"));
	ShipOperDesc.Add("Lock", LOCTEXT("ShipLock", "锁定并显示星球信息"));
	ShipOperDesc.Add("Drive", LOCTEXT("ShipDrive", "脱离飞船"));
}

TArray<FKey> UPlayingHelper::GetActionKeys(FName ActionName)
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

TArray<FKey> UPlayingHelper::GetAxisKeys(FName AxisName)
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

FText UPlayingHelper::GetCharOperDesc(FName Name)
{
	return CharOperDesc[Name];
}

FText UPlayingHelper::GetShipOperDesc(FName Name)
{
	return ShipOperDesc[Name];
}

void UPlayingHelper::GetDebugInfo(TArray<FText>& OutList) const
{
	AMainCharacter* Char = AMainLevelScript::GetMainChar();
	if (Char->FindSpaceship() && Char->FindSpaceship()->GetPilot())
	{
		ASpaceship* Ship = Char->FindSpaceship();
		OutList.Add(FText::Format(INVTEXT("重力: {0} cm/s²"), Ship->GetGravityAccel()));
		OutList.Add(FText::Format(INVTEXT("质量: {0} kg"), Ship->GetMass()));
		OutList.Add(FText::Format(INVTEXT("向前: {0}"), Ship->Engine->GetForwardForce()));
		OutList.Add(FText::Format(INVTEXT("向上: {0}"), Ship->Engine->GetUpForce()));
		OutList.Add(FText::Format(INVTEXT("温度: {0}"), Ship->Body->GetCurrentTemp()));
		OutList.Add(FText::Format(INVTEXT("压力: {0}"), Ship->Body->GetCurrentPress()));
	}
	else
	{
		OutList.Add(FText::Format(INVTEXT("重力: {0} cm/s²"), Char->GetGravityAccel()));
		OutList.Add(FText::Format(INVTEXT("质量: {0} kg"), Char->GetMass()));
		OutList.Add(FText::Format(INVTEXT("向前: {0}"), Char->Engine->GetForwardForce()));
		OutList.Add(FText::Format(INVTEXT("向上: {0}"), Char->Engine->GetUpForce()));
		OutList.Add(FText::Format(INVTEXT("温度: {0}"), Char->Body->GetCurrentTemp()));
		OutList.Add(FText::Format(INVTEXT("压力: {0}"), Char->Body->GetCurrentPress()));
	}
}

#undef LOCTEXT_NAMESPACE
