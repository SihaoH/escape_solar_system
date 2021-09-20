// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickableItemActor.generated.h"

UCLASS()
class ESCAPE_SOLAR_SYSTEM_API APickableItemActor : public AActor
{
	GENERATED_BODY()
	
public:
	APickableItemActor();

	UFUNCTION(BlueprintCallable)
	void Pickup(FName& RowName, int32& Count);

protected:
	virtual void BeginPlay() override;

private:
	void Activate();
	void Deactivate(int32 Time);

public:
	UPROPERTY(Category = PickableItemActor, EditAnywhere, BlueprintReadWrite)
	FName ItemRowName = "-1";

private:
	FTimerHandle RebirthTimer;

	UPROPERTY(Category = PickableItemActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemName;

	UPROPERTY(Category = PickableItemActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(Category = PickableItemActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ScopeTigger = nullptr;

	UPROPERTY(Category = PickableItemActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PromptEffect = nullptr;
};
