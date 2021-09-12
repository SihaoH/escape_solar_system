// Copyright 2020 H₂S. All Rights Reserved.

#include "OverlayWidget.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetBlueprintLibrary.h>

void UOverlayWidget::Close()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	Controller->bShowMouseCursor = false;
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(Controller);
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	RemoveFromParent();
}

void UOverlayWidget::NativeConstruct()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	int32 SizeX, SizeY;
	Controller->GetViewportSize(SizeX, SizeY);
	Controller->SetMouseLocation(SizeX/2, SizeY/2);
	Controller->bShowMouseCursor = true;
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(Controller);
	SetFocus();

	return Super::NativeConstruct();
}

FReply UOverlayWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		Close();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
