// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	void CreateMainChar();
	UFUNCTION(BlueprintCallable)
	void CreateSpaceship();

	UFUNCTION(BlueprintPure)
	class AMainCharacter* FindMainChar() const;
	UFUNCTION(BlueprintPure)
	class ASpaceship* FindSpaceship() const;

public:
	UPROPERTY(Category = EarthBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBackpackComponent* Backpack = nullptr;

protected:
	virtual void BeginPlay() override;

private:
	TSubclassOf<AMainCharacter> BP_MainCharClass;
	TSubclassOf<ASpaceship> BP_SpaceshipClass;

	UPROPERTY(Category = PickableItemActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(Category = EarthBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ScopeTigger = nullptr;
};
