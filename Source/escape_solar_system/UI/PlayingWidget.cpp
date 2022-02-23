// Copyright 2020 H₂S. All Rights Reserved.

#include "PlayingWidget.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "BodyComponent.h"
#include "EngineComponent.h"

void UPlayingWidget::UpdateDebugInfo(TArray<FText>& OutList)
{
	auto* Player = AMainCharacter::GetInstance();
	if (!Player) return;
	OutList.Add(FText::Format(INVTEXT("血量: {0}/{1}"), Player->GetBodyComponent()->CurrentHP, Player->GetBodyComponent()->MaximumHP));
	OutList.Add(FText::Format(INVTEXT("重力: {0} cm/s²"), Player->GetGravityAccel()));
	OutList.Add(FText::Format(INVTEXT("质量: {0} kg"), Player->GetMass()));
	OutList.Add(FText::Format(INVTEXT("功率: {0}"), Player->GetEngineComponent()->Power));
	OutList.Add(FText::Format(INVTEXT("能量: {0}/{1}"), Player->GetEngineComponent()->CurrentEnergy, Player->GetEngineComponent()->MaximumEnergy));
	OutList.Add(FText::Format(INVTEXT("能耗比: {0}"), Player->GetEngineComponent()->EPRatio));
	OutList.Add(FText::Format(INVTEXT("向前: {0}"), Player->GetEngineComponent()->ForwardForce));
	OutList.Add(FText::Format(INVTEXT("向上: {0}"), Player->GetEngineComponent()->UpForce));
	OutList.Add(FText::Format(INVTEXT("耐温: {0}~{1}"), Player->GetBodyComponent()->ShieldCold, Player->GetBodyComponent()->ShieldHeat));
	OutList.Add(FText::Format(INVTEXT("温度: {0}"), Player->GetBodyComponent()->CurrentTemp));
}
