// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MainSaveGame.generated.h"

USTRUCT()
struct FByteArray
{
	GENERATED_USTRUCT_BODY()
	FByteArray() = default;
	FByteArray(const TArray<uint8>& D) : Data(D) {}
	FByteArray(const TArray<uint8, TSizedDefaultAllocator<64>>& D) : Data(D) {}
	TArray<uint8>& operator*() { return Data; }
	const TArray<uint8>& operator*() const { return Data; }

	UPROPERTY()
	TArray<uint8> Data;
};

USTRUCT()
struct FActorData
{
	GENERATED_USTRUCT_BODY()
	FActorData() = default;
	FActorData(const TMap<FString, FByteArray>& D) : Data(D) {}
	TMap<FString, FByteArray>& operator*() { return Data; }
	const TMap<FString, FByteArray>& operator*() const { return Data; }

	UPROPERTY()
	TMap<FString, FByteArray> Data;
};

/**
 * 游戏存档类，存放主要Actor和Component的字节流
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMainSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMainSaveGame();

	UFUNCTION(BlueprintCallable)
	static void SaveAr();
	UFUNCTION(BlueprintCallable)
	static void LoadAr();
	UFUNCTION(BlueprintPure)
	static bool HasAr();

	// 用于跨关卡的变量
	UFUNCTION(BlueprintCallable)
	static void SetNeedLoad(bool Val) { bIsNeedLoad = Val; }
	UFUNCTION(BlueprintCallable)
	static bool IsNeedLoad() { return bIsNeedLoad; }

private:
	friend class AMainLevelScript;
	static int64 SaveTimestamp;
	static bool bIsNeedLoad;
	static FString SaveSlotName;
	static uint32 UserIndex;

	/** 
	* 星球 n
    *  ﹂组件 n
	*/
	UPROPERTY(VisibleAnywhere)
	TMap<FString, FActorData> PlanetList;

	UPROPERTY(VisibleAnywhere)
	TMap<FString, FActorData> MovableActorList;

	UPROPERTY(VisibleAnywhere)
	FActorData MainChar;

	UPROPERTY(VisibleAnywhere)
	FActorData Spaceship;

	UPROPERTY(VisibleAnywhere)
	FActorData EarthBase;

	UPROPERTY(VisibleAnywhere)
	FByteArray PlayerState;
};
