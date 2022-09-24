// Copyright 2020 H₂S. All Rights Reserved.


#include "NPC.h"

ANPC::ANPC()
{
	//PrimaryActorTick.bCanEverTick = true;
	GetMeshComponent()->SetSimulatePhysics(true);
	GetMeshComponent()->SetEnableGravity(false);
	GetMeshComponent()->SetGenerateOverlapEvents(true);
	GetMeshComponent()->SetCollisionProfileName("BlockAllDynamic");
}

void ANPC::SetHighlight(bool bValue)
{
	GetMeshComponent()->SetRenderCustomDepth(bValue);
}

FText ANPC::GetText(FName Index) const
{
	if (Index.IsNone()) {
		return FText();
	}
	return TalkData->FindRow<FTalkData>(Index, FString())->Text;
}

void ANPC::GetOptions(FName Index, TMap<FName, FText>& OutOptions) const
{
	OutOptions = TalkData->FindRow<FTalkData>(Index, FString())->Options;
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	Density = CalcDensity(GetMeshComponent()->BodyInstance.GetMassOverride(), GetMeshComponent()->Bounds.SphereRadius);
}

void ANPC::GravityActed_Implementation(FVector Direction, float Accel)
{
	GetMeshComponent()->AddForce(Direction * Accel, NAME_None, true);
}

void ANPC::GravityActedGlobally_Implementation(FVector Direction, float Accel)
{
	GetMeshComponent()->AddForce(Direction * Accel, NAME_None, true);
}

void ANPC::BuoyancyActed_Implementation(FVector Force)
{
	GetMeshComponent()->AddForce(Force, NAME_None, true);
}

void ANPC::DampingChanged_Implementation(float Linear, float Angular)
{
	FBodyInstance* BI = GetMeshComponent()->GetBodyInstance();
	if (BI)
	{
		BI->LinearDamping = Linear;
		BI->AngularDamping = Angular;
		BI->UpdateDampingProperties();
	}
}
