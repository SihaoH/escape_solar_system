// Copyright 2020 H₂S. All Rights Reserved.

#include "MainLevelScriptActor.h"
#include "MainCharacter.h"
#include "Controllable.h"
#include <JavascriptComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/InputSettings.h>
#include <Blueprint/UserWidget.h>
#include <Blueprint/WidgetBlueprintLibrary.h>

AMainLevelScriptActor* AMainLevelScriptActor::s_Instance = nullptr;

AMainLevelScriptActor::AMainLevelScriptActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	s_Instance = this;
}

void AMainLevelScriptActor::SetMainChar(AMainCharacter* Char)
{
	check(s_Instance->MainChar == nullptr);
	s_Instance->MainChar = Char;
}

void AMainLevelScriptActor::SetSpaceship(ASpaceship* Ship)
{
	if (Ship != nullptr && s_Instance->Spaceship != nullptr)
	{
		// 只能存在一架飞船
		check(false);
	}
	s_Instance->Spaceship = Ship;
}

AMainCharacter* AMainLevelScriptActor::GetMainChar()
{
	return s_Instance->MainChar;
}

ASpaceship* AMainLevelScriptActor::GetSpaceship()
{
	return s_Instance->Spaceship;
}

AEarthBase* AMainLevelScriptActor::GetEarthBase()
{
	check(s_Instance->MainChar);
	return s_Instance->MainChar->FindEarthBase();
}

FActionDoneSignature& AMainLevelScriptActor::AddActionPrompt(FName Action, FText Tag, float Interval)
{
	s_Instance->SetActionPrompt({ Action, Tag, Interval });
	s_Instance->ActionStack.Add({ Action, Tag, Interval });

	return s_Instance->ActionStack.Last().ActionDoneDelegate;
}

void AMainLevelScriptActor::RemoveActionPrompt(FName Action)
{
	for (int Index = s_Instance->ActionStack.Num()-1; Index >= 0; Index--)
	{
		if (s_Instance->ActionStack[Index].Name == Action)
		{
			s_Instance->ActionRemovedDelegate.Broadcast();
			s_Instance->InputComponent->RemoveActionBinding(Action, IE_Pressed);
			s_Instance->InputComponent->RemoveActionBinding(Action, IE_Released);
			s_Instance->ActionStack.RemoveAt(Index);
			if (s_Instance->ActionStack.Num() > 0)
			{
				s_Instance->SetActionPrompt(s_Instance->ActionStack.Last());
			}
			break;
		}
	}
}

void AMainLevelScriptActor::BeginPlay()
{
	check(s_Instance == this);
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
	s_Instance = nullptr;
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
	//UUserWidget* PauseWidget = CreateWidget(GetWorld(), LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UI/WB_Menu.WB_Menu_C'")));
	//PauseWidget->AddToViewport();
	//MainController->bShowMouseCursor = true;
	//UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(MainController);
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
	const ActionInfo& Info = s_Instance->ActionStack.Last();
	Info.ActionDoneDelegate.Broadcast();
	RemoveActionPrompt(Info.Name);
}
