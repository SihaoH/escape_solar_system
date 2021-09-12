// Copyright 2020 H₂S. All Rights Reserved.

#include "MovableMeshActor.h"

AMovableMeshActor::AMovableMeshActor()
{
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetEnableGravity(false);
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
}

void AMovableMeshActor::GravityActed_Implementation(FVector Direction, float Accel)
{
	GetStaticMeshComponent()->AddForce(Direction * Accel, NAME_None, true);
}
