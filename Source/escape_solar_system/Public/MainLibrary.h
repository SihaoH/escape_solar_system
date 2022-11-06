// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "ItemData.h"
#include "TechData.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MainLibrary.generated.h"

/** 翻译数据 */
USTRUCT(BlueprintType)
struct FTranslatedData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/** 原文 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Source;

	/** 翻译后文本 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
	FText Text;
};

UENUM(BlueprintType)
enum class EPawnType : uint8
{
	MainChar,
	SpaceShip
};

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UMainLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UMainLibrary();

public:
	UFUNCTION(BlueprintPure)
	static FORCEINLINE bool IsDev()
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		return true;
#endif
		return false;
	}

	UFUNCTION(BlueprintPure)
	static FText Translate(const FString& Str);

	UFUNCTION(BlueprintCallable)
	static void SendMessage(FText Msg);

	UFUNCTION(BlueprintCallable)
	static FItemData& GetItemData(const FName& RowName);
	static TArray<FName> GetMakeableItemList();

	static FText GetTechValueName(ETech Tech);
	static float GetTechValueVal(ETech Tech, int32 Val);

private:
	static UDataTable* DT_ItemInfo;
	static UDataTable* DT_TechValue;

	static TMap<FString, FText> TranslationMap;
};

#define tr(str) UMainLibrary::Translate(TEXT(str))
