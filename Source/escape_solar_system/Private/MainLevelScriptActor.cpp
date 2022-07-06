// Copyright 2020 H₂S. All Rights Reserved.

#include "MainLevelScriptActor.h"
#include "MainCharacter.h"
#include "Controllable.h"
#include <JavascriptComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/InputSettings.h>
#include <Blueprint/UserWidget.h>
#include <Blueprint/WidgetBlueprintLibrary.h>

AMainLevelScriptActor* AMainLevelScriptActor::ThisInstance = nullptr;

AMainLevelScriptActor::AMainLevelScriptActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	ThisInstance = this;
}

void AMainLevelScriptActor::SetMainChar(AMainCharacter* Char)
{
	// 以这种方式判断主角死亡并发出通知，是不是不太好？
	// 但目前也只有主角死亡时才会Set nullptr
	if (ThisInstance->MainChar != nullptr && Char == nullptr)
	{
		ThisInstance->DeathOpenedDelegate.Broadcast();
	}
	ThisInstance->MainChar = Char;
}

void AMainLevelScriptActor::SetSpaceship(ASpaceship* Ship)
{
	if (Ship != nullptr && ThisInstance->Spaceship != nullptr)
	{
		// 只能存在一架飞船
		check(false);
	}
	ThisInstance->Spaceship = Ship;
}

void AMainLevelScriptActor::SetEarthBase(AEarthBase* Base)
{
	ThisInstance->EarthBase = Base;
}

AMainCharacter* AMainLevelScriptActor::GetMainChar()
{
	return ThisInstance->MainChar;
}

ASpaceship* AMainLevelScriptActor::GetSpaceship()
{
	return ThisInstance->Spaceship;
}

AEarthBase* AMainLevelScriptActor::GetEarthBase()
{
	return ThisInstance->EarthBase;
}

FActionDoneSignature& AMainLevelScriptActor::AddActionPrompt(FName Action, FText Tag, float Interval)
{
	ThisInstance->SetActionPrompt({ Action, Tag, Interval });
	ThisInstance->ActionStack.Add({ Action, Tag, Interval });

	return ThisInstance->ActionStack.Last().ActionDoneDelegate;
}

void AMainLevelScriptActor::RemoveActionPrompt(FName Action)
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

void AMainLevelScriptActor::BeginPlay()
{
	check(ThisInstance == this);
	Super::BeginPlay();

	UJavascriptComponent* Comp = NewObject<UJavascriptComponent>(this, TEXT("MainScript"));
	Comp->ScriptSourceFile = "main.js";
	Comp->RegisterComponent();

	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AMainLevelScriptActor::OnPaused);
	InputComponent->BindAction("Menu", IE_Pressed, this, &AMainLevelScriptActor::OnMenuOpened);
	InputComponent->BindAction("Enter", IE_Pressed, this, &AMainLevelScriptActor::OnEntered);
}

void AMainLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ThisInstance = nullptr;
	MainChar = nullptr;
	IControllable::ClearUp();
}

void AMainLevelScriptActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainLevelScriptActor::SetActionPrompt(const ActionInfo& Info)
{
	if (ActionStack.Num() > 0 && Info.Name != ActionStack.Last().Name)
	{
		FName LastAction = ActionStack.Last().Name;
		InputComponent->RemoveActionBinding(LastAction, IE_Pressed);
		InputComponent->RemoveActionBinding(LastAction, IE_Released);
	}
	InputComponent->BindAction(Info.Name, IE_Pressed, this, &AMainLevelScriptActor::OnActionPressed);
	InputComponent->BindAction(Info.Name, IE_Released, this, &AMainLevelScriptActor::OnActionReleased);

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

void AMainLevelScriptActor::OnPaused()
{
	// 弹出暂停界面
	UUserWidget* PauseWidget = CreateWidget(GetWorld(), LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UI/WB_Pause.WB_Pause_C'")));
	PauseWidget->AddToViewport();
	PausedDelegate.Broadcast();
}

void AMainLevelScriptActor::OnMenuOpened()
{
	MenuOpenedDelegate.Broadcast();
}

void AMainLevelScriptActor::OnEntered()
{
	EnteredDelegate.Broadcast();
}

void AMainLevelScriptActor::OnActionPressed()
{
	ActionPressedDelegate.Broadcast();
}

void AMainLevelScriptActor::OnActionReleased()
{
	ActionReleasedDelegate.Broadcast();
}

void AMainLevelScriptActor::ActionDone()
{
	const ActionInfo& Info = ThisInstance->ActionStack.Last();
	Info.ActionDoneDelegate.Broadcast();
	RemoveActionPrompt(Info.Name);
}
