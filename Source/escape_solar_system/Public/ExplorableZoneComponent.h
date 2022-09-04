﻿// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "ExplorableZoneComponent.generated.h"

/**
 * 可探索的区域
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = SaveGame, meta = (BlueprintSpawnableComponent))
class ESCAPE_SOLAR_SYSTEM_API UExplorableZoneComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UExplorableZoneComponent();
	void Reset();

protected:
	virtual void Serialize(FArchive& Ar) override;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void HandleBonus();

private:
	FTimerHandle Timer;

	UPROPERTY(SaveGame)
	bool IsAvailable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText PlaceName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RequiredTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 BonusPoints = 100;
};
