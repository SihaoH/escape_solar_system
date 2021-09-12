// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "MainLevelScriptActor.generated.h"


UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AMainLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AMainLevelScriptActor(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

private:
	void OnPaused();
	void OnMenuOpened();

private:
	class APlayerController* MainController = nullptr;
};
