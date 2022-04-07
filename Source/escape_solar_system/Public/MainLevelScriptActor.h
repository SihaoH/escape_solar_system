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
	static AMainLevelScriptActor* GetInstance() { return s_Instance; }
	void SetMainChar(class AMainCharacter* Char);

	UFUNCTION(BlueprintPure)
	static class AMainCharacter* GetMainChar();

	UFUNCTION(BlueprintPure)
	static class ASpaceship* GetSpaceship();

	UFUNCTION(BlueprintPure)
	static class AEarthBase* GetEarthBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

private:
	void OnPaused();
	void OnMenuOpened();

private:
	static AMainLevelScriptActor* s_Instance;
	class APlayerController* MainController = nullptr;
	class AMainCharacter* MainChar = nullptr;
};
