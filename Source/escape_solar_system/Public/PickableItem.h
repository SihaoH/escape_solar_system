// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MainSaveGame.h"
#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PickableItem.generated.h"

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ESCAPE_SOLAR_SYSTEM_API UPickableItem : public UStaticMeshComponent, public ISaveGameClass
{
	GENERATED_BODY()
	
public:
	UPickableItem();

	FORCEINLINE void SetHighlight(bool bValue)
	{
		SetRenderCustomDepth(bValue);
	}

	UFUNCTION(BlueprintCallable)
	void Pickup(FName& RowName, int32& Count);

protected:
	virtual void Serialize(FArchive& Ar) override;
	virtual void BeginPlay() override;

private:
	void Enable();
	void Disable(int32 Time);
	void OnCountdown();

public:
	UPROPERTY(Category = PickableItemActor, EditAnywhere, BlueprintReadWrite)
	FName ItemRowName = "-1";

private:
	FTimerHandle RebirthTimer;

	UPROPERTY(SaveGame)
	int32 Countdown = 0;

	UPROPERTY(Category = PickableItemActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemName;

	UPROPERTY(Category = PickableItemActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PromptEffect = nullptr;
};
