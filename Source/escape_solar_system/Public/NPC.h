// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassActorInterface.h"
#include "Engine/DataTable.h"
#include "GameFramework/DefaultPawn.h"
#include "NPC.generated.h"

/** 对话数据 */
USTRUCT(BlueprintType)
struct FTalkData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	// 对白文本
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	// 对话选项，有三种情况：1.选项有文本，需要玩家点击选择 2.选项只有一个且无文本，自动跳到对应的行 3.无选项，即结束对话
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FText> Options;
};

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API ANPC : public ADefaultPawn, public IMassActorInterface
{
	GENERATED_BODY()

public:
	ANPC();

	FORCEINLINE void SetHighlight(bool bValue);

	FORCEINLINE FText GetName() const { return Name; };

	UFUNCTION(BlueprintPure)
	FText GetText(FName Index) const;

	UFUNCTION(BlueprintPure)
	void GetOptions(FName Index, TMap<FName, FText>& OutOptions) const;

protected:
	virtual void BeginPlay() override;
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;
	virtual void GravityActedGlobally_Implementation(FVector Direction, float Accel) override;
	virtual void BuoyancyActed_Implementation(FVector Force) override;
	virtual void DampingChanged_Implementation(float Linear, float Angular) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText Name;

	UPROPERTY(EditAnywhere, meta = (RequiredAssetDataTags = "RowStructure=TalkData"))
	TObjectPtr<class UDataTable> TalkData;
};
