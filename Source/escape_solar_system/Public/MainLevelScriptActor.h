// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "MainLevelScriptActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPausedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuOpenedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnteredSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessagedSignature, FText, Msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExplorePointsSignature, FText, Msg);

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AMainLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AMainLevelScriptActor(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure)
	static AMainLevelScriptActor* Instance() { return s_Instance; }

	void SetMainChar(class AMainCharacter* Char);

	UFUNCTION(BlueprintPure)
	static class AMainCharacter* GetMainChar();

	UFUNCTION(BlueprintPure)
	static class ASpaceship* GetSpaceship();

	UFUNCTION(BlueprintPure)
	static class AEarthBase* GetEarthBase();

public:
	UPROPERTY(BlueprintAssignable)
	FPausedSignature PausedDelegate;

	UPROPERTY(BlueprintAssignable)
	FMenuOpenedSignature MenuOpenedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FEnteredSignature EnteredDelegate;

	UPROPERTY(BlueprintAssignable)
	FMessagedSignature MessagedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FExplorePointsSignature ExplorePointsDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

private:
	void OnPaused();
	void OnMenuOpened();
	void OnEntered();

private:
	static AMainLevelScriptActor* s_Instance;
	class APlayerController* MainController = nullptr;
	class AMainCharacter* MainChar = nullptr;
};
