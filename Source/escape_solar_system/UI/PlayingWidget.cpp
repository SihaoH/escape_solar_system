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
	//OutList.Add(FText::Format(INVTEXT("血量: {0}/{1}"), Player->Body->CurrentHP, Player->Body->MaximumHP));
	//OutList.Add(FText::Format(INVTEXT("重力: {0} cm/s²"), Player->GetGravityAccel()));
	//OutList.Add(FText::Format(INVTEXT("质量: {0} kg"), Player->GetMass()));
	//OutList.Add(FText::Format(INVTEXT("功率: {0}"), Player->Engine->Power));
	//OutList.Add(FText::Format(INVTEXT("能量: {0}/{1}"), Player->Engine->CurrentEnergy, Player->Engine->MaximumEnergy));
	//OutList.Add(FText::Format(INVTEXT("能耗比: {0}"), Player->Engine->EPRatio));
	//OutList.Add(FText::Format(INVTEXT("向前: {0}"), Player->Engine->ForwardForce));
	//OutList.Add(FText::Format(INVTEXT("向上: {0}"), Player->Engine->UpForce));
	//OutList.Add(FText::Format(INVTEXT("耐温: {0}~{1}"), Player->Body->ShieldCold, Player->Body->ShieldHeat));
	//OutList.Add(FText::Format(INVTEXT("温度: {0}"), Player->Body->CurrentTemp));
}
