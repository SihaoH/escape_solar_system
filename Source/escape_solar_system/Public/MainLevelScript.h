// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "MainLevelScript.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCinematicsSignature, FString, File);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessagedSignature, FText, Msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExplorePointsSignature, FText, Msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCelestialBodySignature, class ACelestialBody*, Body);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNPCSpeakSignature, FText, Name, FText, Text, float, Time);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FActionAddedSignature, FKey, Key, FText, Tag, float, Interval);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionSignature);


UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AMainLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	struct ActionInfo
	{
		FName Name;
		FText Tag;
		float Interval = 0;
		FActionSignature ActionDoneDelegate;
	};

public:
	AMainLevelScript(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure)
	static AMainLevelScript* Instance() { return ThisInstance; }

	static void SetMainChar(class AMainCharacter* Char);
	static void SetSpaceship(class ASpaceship* Ship);
	static void SetEarthBase(class AEarthBase* Base);

	UFUNCTION(BlueprintPure)
	static class AMainCharacter* GetMainChar();

	UFUNCTION(BlueprintPure)
	static class ASpaceship* GetSpaceship();

	UFUNCTION(BlueprintPure)
	static class AEarthBase* GetEarthBase();

	static void LookCelestialBody(class ACelestialBody* Body);
	static void LockCelestialBody(class ACelestialBody* Body);

	static FActionSignature& AddActionPrompt(FName Action, FText Tag, float Interval = 0);
	static void RemoveActionPrompt(FName Action);

public:
	UPROPERTY(BlueprintAssignable)
	FCinematicsSignature CinematicsDelegate;

	UPROPERTY(BlueprintAssignable)
	FInteractionSignature PlayingDelegate;

	UPROPERTY(BlueprintAssignable)
	FInteractionSignature PausedDelegate;

	UPROPERTY(BlueprintAssignable)
	FInteractionSignature MenuOpenedDelegate;

	UPROPERTY(BlueprintAssignable)
	FInteractionSignature DeathOpenedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FInteractionSignature TalkOpenedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FInteractionSignature EnteredDelegate;

	UPROPERTY(BlueprintAssignable)
	FMessagedSignature MessagedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FExplorePointsSignature ExplorePointsDelegate;

	UPROPERTY(BlueprintAssignable)
	FCelestialBodySignature CelestialBodyLookedDelegate;

	UPROPERTY(BlueprintAssignable)
	FCelestialBodySignature CelestialBodyLockedDelegate;

	UPROPERTY(BlueprintAssignable)
	FNPCSpeakSignature NPCSpeakDelegate;

	UPROPERTY(BlueprintAssignable)
	FActionAddedSignature ActionAddedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FActionSignature ActionRemovedDelegate;

	UPROPERTY(BlueprintAssignable)
	FActionSignature ActionPressedDelegate;

	UPROPERTY(BlueprintAssignable)
	FActionSignature ActionReleasedDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

private:
	void SetActionPrompt(const ActionInfo& Info);
	void OnPaused();
	void OnMenuOpened();
	void OnEntered();
	void OnActionPressed();
	void OnActionReleased();
	UFUNCTION()
	void OnSequenceFinshed();

	UFUNCTION(BlueprintCallable)
	static void ActionDone();

private:
	static AMainLevelScript* ThisInstance;
	
	TArray<ActionInfo> ActionStack;
	class APlayerController* MainController = nullptr;
	class AMainCharacter* MainChar = nullptr;
	class ASpaceship* Spaceship = nullptr;
	class AEarthBase* EarthBase = nullptr;
	class ALevelSequenceActor* LevelSequence = nullptr;
};
