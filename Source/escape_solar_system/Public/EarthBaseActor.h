// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EarthBaseActor.generated.h"

/**
 * 地球基地类，整个场景只会存在一个，相当于“家”
 */
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API AEarthBaseActor : public AActor
{
	GENERATED_BODY()

public:
	AEarthBaseActor();

	class ASpaceship* FindSpaceship() const;
	class UBackpackComponent* GetBackpack() const { return Storehouse; }

protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY(Category = EarthBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(Category = EarthBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TiggerComponent;

	UPROPERTY(Category = EarthBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBackpackComponent* Storehouse = nullptr;
};
