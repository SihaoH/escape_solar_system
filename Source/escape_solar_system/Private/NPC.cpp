// Copyright 2020 H₂S. All Rights Reserved.

#include "NPC.h"
#include "MainPlayerState.h"
#include <Components/SphereComponent.h>
#include <GameFramework/PawnMovementComponent.h>
#include <GameFramework/FloatingPawnMovement.h>
#include <GameFramework/PlayerInput.h>

ANPC::ANPC()
{
	//PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetEnableGravity(false);
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionProfileName("BlockAllDynamic");
	SetRootComponent(MeshComponent);

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = MeshComponent;
}

void ANPC::SetHighlight(bool bValue)
{
	MeshComponent->SetRenderCustomDepth(bValue);
}

FText ANPC::GetText(FName Index) const
{
	if (Index.IsNone()) {
		return FText();
	}
	return FText::Format(TalkData->FindRow<FTalkData>(Index, FString())->Text, GetVariableArguments());
}

void ANPC::GetOptions(FName Index, TArray<FOptionData>& OutOptions) const
{
	OutOptions = TalkData->FindRow<FTalkData>(Index, FString())->Options;
}

void ANPC::BeginPlay()
{
	Super::BeginPlay(); // AActor::BeginPlay()会判断附着的父对象是否模拟物理，如果不一致，就解除附着
	// 所以模拟物理要在BeginPlay之后开启，而且也不能用UPrimitiveComponent::SetSimulatePhysics开启，因为也会像BeginPlay那样做
	MeshComponent->GetBodyInstance()->bSimulatePhysics = true;
	MeshComponent->GetBodyInstance()->UpdateInstanceSimulatePhysics();

	Density = CalcDensity(MeshComponent->BodyInstance.GetMassOverride(), MeshComponent->Bounds.SphereRadius);
}

void ANPC::GravityActed_Implementation(FVector Direction, float Accel)
{
	MeshComponent->AddForce(Direction * Accel, NAME_None, true);
}

void ANPC::GravityActedGlobally_Implementation(FVector Direction, float Accel)
{
	MeshComponent->AddForce(Direction * Accel, NAME_None, true);
}

void ANPC::BuoyancyActed_Implementation(FVector Force)
{
	MeshComponent->AddForce(Force, NAME_None, true);
}

void ANPC::DampingChanged_Implementation(float Linear, float Angular)
{
	FBodyInstance* BI = MeshComponent->GetBodyInstance();
	if (BI)
	{
		BI->LinearDamping = Linear;
		BI->AngularDamping = Angular;
		BI->UpdateDampingProperties();
	}
}

FFormatNamedArguments ANPC::GetVariableArguments() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("DeathCount"), FText::AsNumber(AMainPlayerState::Instance()->GetDeathCount() + 789));
	return Args;
}
