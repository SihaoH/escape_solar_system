// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
#include <Components/SphereComponent.h>
#include "EarthBaseActor.generated.h"

/**
 * 地球基地类，整个场景只会存在一个，相当于“家”
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AEarthBase : public AActor
{
	GENERATED_BODY()

public:
	AEarthBase();

	UFUNCTION(BlueprintCallable)
	void CreateMainChar(bool bLoadAr = false);
	UFUNCTION(BlueprintCallable)
	void CreateSpaceship();

	UFUNCTION(BlueprintPure)
	class AMainCharacter* FindMainChar() const;
	UFUNCTION(BlueprintPure)
	class ASpaceship* FindSpaceship() const;

	FORCEINLINE UShapeComponent* GetCharScope() const { return CharScope; }
	FORCEINLINE UShapeComponent* GetShipScope() const { return ShipScope; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnShipEnter();
	UFUNCTION(BlueprintImplementableEvent)
	void OnShipLeave();

private:
	UFUNCTION()
	virtual void OnShipScopeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnShipScopeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(Category = "基地", VisibleAnywhere, BlueprintReadOnly)
	class UBackpackComponent* Backpack = nullptr;

private:
	TSubclassOf<class AMainCharacter> BP_MainCharClass;
	TSubclassOf<class ASpaceship> BP_SpaceshipClass;
	TSubclassOf<class ANPC> BP_GuideNPCClass;
	TObjectPtr<class ANPC> GuideNPC;

	UPROPERTY(Category = "基地", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CharScope = nullptr;

	UPROPERTY(Category = "基地", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* ShipScope = nullptr;

	/** 主角的出生点 */
	UPROPERTY(Category = "基地", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTransform CharBirthplace;

	/** 飞船的出生点 */
	UPROPERTY(Category = "基地", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTransform ShipBirthplace;
};
