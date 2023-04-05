// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "MainSaveGame.h"
#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "ExplorableZoneComponent.generated.h"

/**
 * 可探索的区域
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ESCAPE_SOLAR_SYSTEM_API UExplorableZoneComponent : public USphereComponent, public ISaveGameClass
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

	UPROPERTY(Category = "奖励点区域", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText PlaceName;

	UPROPERTY(Category = "奖励点区域", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RequiredTime = 5.0f;

	UPROPERTY(Category = "奖励点区域", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 BonusPoints = 100;
};
