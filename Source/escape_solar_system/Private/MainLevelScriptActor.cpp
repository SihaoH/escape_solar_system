// Copyright 2020 H₂S. All Rights Reserved.

#include "MainLevelScriptActor.h"
#include "MainCharacter.h"
#include "Controllable.h"
#include <JavascriptComponent.h>
#include <Kismet/GameplayStatics.h>
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
	check(MainChar == nullptr);
	MainChar = Char;
}

void AMainLevelScriptActor::SetSpaceship(ASpaceship* Ship)
{
	if (Ship != nullptr && Spaceship != nullptr)
	{
		// 只能存在一架飞船
		check(false);
	}
	Spaceship = Ship;
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

void AMainLevelScriptActor::BeginPlay()
{
	check(s_Instance == this);
	Super::BeginPlay();

	UJavascriptComponent* comp = NewObject<UJavascriptComponent>(this, TEXT("MainScript"));
	comp->ScriptSourceFile = "main.js";
	comp->RegisterComponent();
	//UUserWidget* PlayingWidget = CreateWidget(GetWorld(), LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UI/WB_Playing.WB_Playing_C'")));
	//PlayingWidget->AddToViewport();

	//MainController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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
