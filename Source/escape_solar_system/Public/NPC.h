// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassActorInterface.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"
#include "NPC.generated.h"

USTRUCT(BlueprintType)
struct FOptionData
{
	GENERATED_USTRUCT_BODY()

	/** 升级所需的点数 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Row;

	/** 升级所需的物品 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Text;
};

/** 对话数据 */
USTRUCT(BlueprintType)
struct FTalkData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/** 对白文本 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
	FText Text;

	/** 对话选项，有三种情况：1.选项有文本，需要玩家点击选择 2.选项只有一个且无文本，自动跳到对应的行 3.无选项，即结束对话 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FOptionData> Options;
};

/** 自发言数据 */
USTRUCT(BlueprintType)
struct FSpeakData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/** 发言文本 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
	FText Text;

	/** 发言显示时长 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time = 3.f;
};

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API ANPC : public APawn, public IMassActorInterface
{
	GENERATED_BODY()

public:
	ANPC();

	FORCEINLINE void SetHighlight(bool bValue)
	{
		MeshComponent->SetRenderCustomDepth(bValue);
	}

	FORCEINLINE FText GetName() const { return Name; };

	UFUNCTION(BlueprintPure)
	FText GetText(FName Index) const;

	UFUNCTION(BlueprintPure)
	void GetOptions(FName Index, TArray<FOptionData>& OutOptions) const;

protected:
	virtual void BeginPlay() override;
	virtual void GravityActed_Implementation(FVector Direction, float Accel) override;
	virtual void GravityActedGlobally_Implementation(FVector Direction, float Accel) override;
	virtual void BuoyancyActed_Implementation(FVector Force) override;
	virtual void DampingChanged_Implementation(float Linear, float Angular) override;

private:
	UFUNCTION()
	void OnSpeakZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSpeakZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SpeakNext();
	
	FORCEINLINE FFormatNamedArguments GetVariableArguments() const;

protected:
	UPROPERTY(Category = "NPC", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(Category = "NPC", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> SpeakZone;

	UPROPERTY(Category = "NPC", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPawnMovementComponent> MovementComponent;

private:
	bool bSimulatePhysics = false;
	FTimerHandle SpeakTimer;
	TArray<FSpeakData*> SpeakList;

	UPROPERTY(Category = "NPC", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText Name;

	UPROPERTY(Category = "NPC", EditAnywhere, meta = (RequiredAssetDataTags = "RowStructure=TalkData"))
	TObjectPtr<class UDataTable> TalkData;

	UPROPERTY(Category = "NPC", EditAnywhere, meta = (RequiredAssetDataTags = "RowStructure=SpeakData"))
	TObjectPtr<class UDataTable> SpeakData;
};
