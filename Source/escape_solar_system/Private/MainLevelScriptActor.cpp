// Copyright 2020 H₂S. All Rights Reserved.

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
	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AMainLevelScriptActor::OnPaused);
	InputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &AMainLevelScriptActor::OnMenuOpened);
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
	UUserWidget* PauseWidget = CreateWidget(GetWorld(), LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UI/WB_Pause.WB_Pause_C'")));
	PauseWidget->AddToViewport();
}

void AMainLevelScriptActor::OnMenuOpened()
{
	UUserWidget* PauseWidget = CreateWidget(GetWorld(), LoadClass<UUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/UI/WB_Menu.WB_Menu_C'")));
	PauseWidget->AddToViewport();
	MainController->bShowMouseCursor = true;
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(MainController);
}
