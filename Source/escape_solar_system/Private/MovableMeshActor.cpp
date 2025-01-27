﻿// Copyright 2020 H₂S. All Rights Reserved.

#include "MovableMeshActor.h"

AMovableMeshActor::AMovableMeshActor()
{
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetEnableGravity(false);
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);

	// 作为独立的Actor时，此属性须为true，关卡开始时已在某碰撞范围内的Actor才会触发重叠事件
	bGenerateOverlapEventsDuringLevelStreaming = true;
}

void AMovableMeshActor::BeginPlay()
{
	Super::BeginPlay();
	Density = CalcDensity(GetStaticMeshComponent()->BodyInstance.GetMassOverride(), GetStaticMeshComponent()->Bounds.SphereRadius);
}

void AMovableMeshActor::GravityActed_Implementation(FVector Direction, float Accel)
{
	GetStaticMeshComponent()->AddForce(Direction * Accel, NAME_None, true);
}

void AMovableMeshActor::GravityActedGlobally_Implementation(FVector Direction, float Accel)
{
	GetStaticMeshComponent()->AddForce(Direction * Accel, NAME_None, true);
}

void AMovableMeshActor::BuoyancyActed_Implementation(FVector Force)
{
	GetStaticMeshComponent()->AddForce(Force, NAME_None, true);
}

void AMovableMeshActor::DampingChanged_Implementation(float Linear, float Angular)
{
	FBodyInstance* BI = GetStaticMeshComponent()->GetBodyInstance();
	if (BI)
	{
		BI->LinearDamping = Linear;
		BI->AngularDamping = Angular;
		BI->UpdateDampingProperties();
	}
}
