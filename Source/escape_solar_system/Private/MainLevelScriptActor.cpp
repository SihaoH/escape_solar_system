﻿// Copyright 2020 H₂S. All Rights Reserved.

#include "MainLevelScriptActor.h"
#include "Controllable.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/UserWidget.h>
#include <Blueprint/WidgetBlueprintLibrary.h>


AMainLevelScriptActor::AMainLevelScriptActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMainLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* PlayingWidget = CreateWidget(GetWorld(), LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UI/WB_Playing.WB_Playing_C'")));
	PlayingWidget->AddToViewport();

	MainController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	// 在编辑器中ESC被占用，所以暂时使用F12；发布时要改成ESC键
	InputComponent->BindKey(EKeys::F12, IE_Pressed, this, &AMainLevelScriptActor::OnPaused);
}

void AMainLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	IControllable::ClearUp();
}

void AMainLevelScriptActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainLevelScriptActor::OnPaused()
{
	// 弹出暂停界面
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	UUserWidget* PauseWidget = CreateWidget(GetWorld(), LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UI/WB_Pause.WB_Pause_C'")));
	PauseWidget->AddToViewport();
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	Controller->bShowMouseCursor = true;
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(Controller);
}