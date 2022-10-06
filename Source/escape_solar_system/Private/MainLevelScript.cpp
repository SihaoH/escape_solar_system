// Copyright 2020 H₂S. All Rights Reserved.

#include "MainLevelScript.h"
#include "MainCharacter.h"
#include "EarthBaseActor.h"
#include "Controllable.h"
#include "MainPlayerState.h"
#include "MainSaveGame.h"
#include <JavascriptComponent.h>
#include <LevelSequence/Public/LevelSequencePlayer.h>
#include <LevelSequence/Public/LevelSequenceActor.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/InputSettings.h>
#include <Blueprint/UserWidget.h>
#include <Blueprint/WidgetBlueprintLibrary.h>

AMainLevelScript* AMainLevelScript::ThisInstance = nullptr;

AMainLevelScript::AMainLevelScript(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	ThisInstance = this;
}

void AMainLevelScript::SetMainChar(AMainCharacter* Char)
{
	// 以这种方式判断主角死亡并发出通知，是不是不太好？
	// 但目前也只有主角死亡时才会Set nullptr
	if (ThisInstance->MainChar != nullptr && Char == nullptr)
	{
		ThisInstance->DeathOpenedDelegate.Broadcast();
	}
	ThisInstance->MainChar = Char;
}

void AMainLevelScript::SetSpaceship(ASpaceship* Ship)
{
	if (Ship != nullptr && ThisInstance->Spaceship != nullptr)
	{
		// 只能存在一架飞船
		check(false);
	}
	ThisInstance->Spaceship = Ship;
}

void AMainLevelScript::SetEarthBase(AEarthBase* Base)
{
	ThisInstance->EarthBase = Base;
}

AMainCharacter* AMainLevelScript::GetMainChar()
{
	return ThisInstance->MainChar;
}

ASpaceship* AMainLevelScript::GetSpaceship()
{
	return ThisInstance->Spaceship;
}

AEarthBase* AMainLevelScript::GetEarthBase()
{
	return ThisInstance->EarthBase;
}

void AMainLevelScript::LookCelestialBody(ACelestialBody* Body)
{
	ThisInstance->CelestialBodyLookedDelegate.Broadcast(Body);
}

void AMainLevelScript::LockCelestialBody(ACelestialBody* Body)
{
	ThisInstance->CelestialBodyLockedDelegate.Broadcast(Body);
}

FActionSignature& AMainLevelScript::AddActionPrompt(FName Action, FText Tag, float Interval)
{
	ThisInstance->SetActionPrompt({ Action, Tag, Interval });
	ThisInstance->ActionStack.Add({ Action, Tag, Interval });

	return ThisInstance->ActionStack.Last().ActionDoneDelegate;
}

void AMainLevelScript::RemoveActionPrompt(FName Action)
{
	for (int Index = ThisInstance->ActionStack.Num()-1; Index >= 0; Index--)
	{
		if (ThisInstance->ActionStack[Index].Name == Action)
		{
			ThisInstance->ActionRemovedDelegate.Broadcast();
			ThisInstance->InputComponent->RemoveActionBinding(Action, IE_Pressed);
			ThisInstance->InputComponent->RemoveActionBinding(Action, IE_Released);
			ThisInstance->ActionStack.RemoveAt(Index);
			if (ThisInstance->ActionStack.Num() > 0)
			{
				ThisInstance->SetActionPrompt(ThisInstance->ActionStack.Last());
			}
			break;
		}
	}
}

void AMainLevelScript::BeginPlay()
{
	check(ThisInstance == this);
	Super::BeginPlay();

	UJavascriptComponent* Comp = NewObject<UJavascriptComponent>(this, TEXT("MainScript"));
	Comp->ScriptSourceFile = "main.js";
	Comp->RegisterComponent();

	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AMainLevelScript::OnPaused);
	InputComponent->BindAction("Menu", IE_Pressed, this, &AMainLevelScript::OnMenuOpened);
	InputComponent->BindAction("Enter", IE_Pressed, this, &AMainLevelScript::OnEntered);

	// 第一次进入游戏，播放开场动画
	if (AMainPlayerState::Instance()->GetTotalTime() < 1.f)
	{
		ULevelSequence* SequenceAsset = Cast<ULevelSequence>(FStringAssetReference("LevelSequence'/Game/Cinematics/LS1.LS1'").TryLoad());
		ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), SequenceAsset, FMovieSceneSequencePlaybackSettings(), LevelSequence);
		SequencePlayer->OnFinished.AddDynamic(this, &AMainLevelScript::OnSequenceFinshed);
		SequencePlayer->Play();
		// TODO 改成外部配置
		CinematicsDelegate.Broadcast(TEXT("cinematics_1"));
	}
	else
	{
		GetEarthBase()->CreateMainChar();
		PlayingDelegate.Broadcast();
	}

	// 加载游戏存档
	UMainSaveGame::SaveTimestamp = FDateTime::UtcNow().ToUnixTimestamp();
	if (UMainSaveGame::IsNeedLoad())
	{
		UMainSaveGame::LoadAr();
	}
}

void AMainLevelScript::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ThisInstance = nullptr;
	MainChar = nullptr;
	IControllable::ClearUp();
}

void AMainLevelScript::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainLevelScript::SetActionPrompt(const ActionInfo& Info)
{
	if (ActionStack.Num() > 0 && Info.Name != ActionStack.Last().Name)
	{
		FName LastAction = ActionStack.Last().Name;
		InputComponent->RemoveActionBinding(LastAction, IE_Pressed);
		InputComponent->RemoveActionBinding(LastAction, IE_Released);
	}
	InputComponent->BindAction(Info.Name, IE_Pressed, this, &AMainLevelScript::OnActionPressed);
	InputComponent->BindAction(Info.Name, IE_Released, this, &AMainLevelScript::OnActionReleased);

	auto InputSettings = UInputSettings::GetInputSettings();
	TArray<FInputActionKeyMapping> Mappings;
	InputSettings->GetActionMappingByName(Info.Name, Mappings);
	FKey Key;
	for (const auto& KeyMap : Mappings)
	{
		Key = KeyMap.Key;
		break;
	}
	ActionAddedDelegate.Broadcast(Key, Info.Tag, Info.Interval);
}

void AMainLevelScript::OnPaused()
{
	if (LevelSequence)
	{
		OnSequenceFinshed();
	}
	else
	{
		// 弹出暂停界面
		PausedDelegate.Broadcast();
	}
}

void AMainLevelScript::OnMenuOpened()
{
	MenuOpenedDelegate.Broadcast();
}

void AMainLevelScript::OnEntered()
{
	EnteredDelegate.Broadcast();
}

void AMainLevelScript::OnActionPressed()
{
	ActionPressedDelegate.Broadcast();
}

void AMainLevelScript::OnActionReleased()
{
	ActionReleasedDelegate.Broadcast();
}

void AMainLevelScript::OnSequenceFinshed()
{
	if (LevelSequence)
	{
		LevelSequence->Destroy();
		LevelSequence = nullptr;

		CinematicsDelegate.Broadcast(TEXT(""));
		GetEarthBase()->CreateMainChar();
		PlayingDelegate.Broadcast();
	}
}

void AMainLevelScript::ActionDone()
{
	const ActionInfo& Info = ThisInstance->ActionStack.Last();
	Info.ActionDoneDelegate.Broadcast();
	RemoveActionPrompt(Info.Name);
}
