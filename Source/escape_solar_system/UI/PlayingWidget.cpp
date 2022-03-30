// Copyright 2020 H₂S. All Rights Reserved.

#include "PlayingWidget.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"

void UPlayingHelper::GetDebugInfo(TArray<FText>& OutList) const
{
	AMainCharacter* Char = AMainCharacter::GetInstance();
	if (!Char) return;
	OutList.Add(FText::Format(INVTEXT("血量: {0}/{1}"), Char->Body->GetCurrentHP(), Char->Body->GetMaximumHP()));
	OutList.Add(FText::Format(INVTEXT("重力: {0} cm/s²"), Char->GetGravityAccel()));
	OutList.Add(FText::Format(INVTEXT("质量: {0} kg"), Char->GetMass()));
	OutList.Add(FText::Format(INVTEXT("功率: {0}"), Char->Engine->GetPower()));
	OutList.Add(FText::Format(INVTEXT("能量: {0}/{1}"), Char->Engine->GetCurrentEnergy(), Char->Engine->GetMaximumEnergy()));
	OutList.Add(FText::Format(INVTEXT("能耗比: {0}"), Char->Engine->GetEPRatio()));
	OutList.Add(FText::Format(INVTEXT("能质比: {0}"), Char->Engine->GetEMRatio()));
	OutList.Add(FText::Format(INVTEXT("向前: {0}"), Char->Engine->GetForwardForce()));
	OutList.Add(FText::Format(INVTEXT("向上: {0}"), Char->Engine->GetUpForce()));
	OutList.Add(FText::Format(INVTEXT("耐温: {0}~{1}"), Char->Body->GetShieldCold(), Char->Body->GetShieldHeat()));
	OutList.Add(FText::Format(INVTEXT("温度: {0}"), Char->Body->GetCurrentTemp()));
}
