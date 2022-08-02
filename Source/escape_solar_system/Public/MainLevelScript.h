// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "MainLevelScript.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPausedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuOpenedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathOpenedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTalkOpenedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnteredSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessagedSignature, FText, Msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExplorePointsSignature, FText, Msg);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FActionAddedSignature, FKey, Key, FText, Tag, float, Interval);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionRemovedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionPressedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionReleasedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionDoneSignature);


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
		FActionDoneSignature ActionDoneDelegate;
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

	static FActionDoneSignature& AddActionPrompt(FName Action, FText Tag, float Interval = 0);
	static void RemoveActionPrompt(FName Action);

public:
	UPROPERTY(BlueprintAssignable)
	FPausedSignature PausedDelegate;

	UPROPERTY(BlueprintAssignable)
	FMenuOpenedSignature MenuOpenedDelegate;

	UPROPERTY(BlueprintAssignable)
	FDeathOpenedSignature DeathOpenedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FTalkOpenedSignature TalkOpenedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FEnteredSignature EnteredDelegate;

	UPROPERTY(BlueprintAssignable)
	FMessagedSignature MessagedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FExplorePointsSignature ExplorePointsDelegate;

	UPROPERTY(BlueprintAssignable)
	FActionAddedSignature ActionAddedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FActionRemovedSignature ActionRemovedDelegate;

	UPROPERTY(BlueprintAssignable)
	FActionPressedSignature ActionPressedDelegate;

	UPROPERTY(BlueprintAssignable)
	FActionReleasedSignature ActionReleasedDelegate;

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

	UFUNCTION(BlueprintCallable)
	static void ActionDone();

private:
	static AMainLevelScript* ThisInstance;
	TArray<ActionInfo> ActionStack;
	class APlayerController* MainController = nullptr;
	class AMainCharacter* MainChar = nullptr;
	class ASpaceship* Spaceship = nullptr;
	class AEarthBase* EarthBase = nullptr;
};
