// Copyright 2020 H₂S. All Rights Reserved.

#include "PlayingWidget.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"

void UPlayingHelper::GetDebugInfo(TArray<FText>& OutList) const
{
	AMainCharacter* Char = AMainLevelScriptActor::GetMainChar();
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
