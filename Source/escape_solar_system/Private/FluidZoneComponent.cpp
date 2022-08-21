// Copyright 2020 H₂S. All Rights Reserved.


#include "FluidZoneComponent.h"
#include "MassActorInterface.h"
#include "PlanetActor.h"
#include <Components/SphereComponent.h>


UFluidZoneComponent::UFluidZoneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetGenerateOverlapEvents(true);
	SetCollisionProfileName("OverlapAllDynamic");

	OnComponentBeginOverlap.AddDynamic(this, &UFluidZoneComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UFluidZoneComponent::OnEndOverlap);

	PostProcess = CreateDefaultSubobject<UMeshPostProcessComponent>(TEXT("PostProcess"));
	PostProcess->bUnbound = false;
	PostProcess->SetupAttachment(this);
}

float UFluidZoneComponent::HeatTransfer(float Current) const
{
	constexpr float Volatility = 3.f;
	const float diff = Temperature - Current;
	if (FMath::Abs(diff) < Volatility)
	{
		return Current + (FMath::FRand() - 0.5) * Volatility * 2;
	}
	else
	{
		return Current + diff * Efficiency;
	}
}

float UFluidZoneComponent::PressTransfer(FVector Location) const
{
	check(Bounds.SphereRadius > 0);
	const float CenterDist = Bounds.SphereRadius * (1 - BlendDistance);
	float Distance = FVector::Distance(GetComponentLocation(), Location);
	if (Distance > CenterDist)
	{
		float Value = (1 - (Distance - CenterDist) / (Bounds.SphereRadius * BlendDistance)) * Pressure;
		return Value > 0 ? Value : 0;
	}
	return Pressure;
}

void UFluidZoneComponent::BeginPlay()
{
	Super::BeginPlay();
	AttachedPlanet = Cast<APlanetActor>(GetOwner());
	check(AttachedPlanet);
}

void UFluidZoneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TSet<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
		{
			auto MassActor = Cast<IMassActorInterface>(Actor);
			if (MassActor->FluidZoneStack.Last() == this && FVector::Dist(Actor->GetActorLocation(), GetComponentLocation()) < Bounds.SphereRadius)
			{
				FVector Direction = FVector::ZeroVector;
				float Accel = 0;
				AttachedPlanet->CalcGravityResult(Actor, Direction, Accel);
				Accel *= MassActor->Buoyancy * -1;

				IMassActorInterface::Execute_BuoyancyActed(Actor, Direction*Accel);
			}
		}
	}
}

void UFluidZoneComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
	{
		auto Target = Cast<IMassActorInterface>(OtherActor);
		Target->FluidZoneStack.AddUnique(this);
		IMassActorInterface::Execute_DampingChanged(OtherActor, LinerDamping, AngularDamping);
	}
}

void UFluidZoneComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UMassActorInterface::StaticClass()))
	{
		auto MassActor = Cast<IMassActorInterface>(OtherActor);
		check(MassActor->FluidZoneStack.Last() == this);
		MassActor->FluidZoneStack.RemoveSingle(this);
		if (MassActor->FluidZoneStack.Num() > 0)
		{
			IMassActorInterface::Execute_DampingChanged(OtherActor, MassActor->FluidZoneStack.Last()->LinerDamping, MassActor->FluidZoneStack.Last()->AngularDamping);
		}
		else
		{
			IMassActorInterface::Execute_DampingChanged(OtherActor, 0, 0);
		}
	}
}

bool UMeshPostProcessComponent::EncompassesPoint(FVector Point, float SphereRadius, float* OutDistanceToPoint)
{
	UFluidZoneComponent* ParentShape = Cast<UFluidZoneComponent>(GetAttachParent());
	if (ParentShape != nullptr)
	{
		float Distance = -1.f;

		FVector ClosestPoint;
		float DistanceSq = -1.f;

		if (ParentShape->GetSquaredDistanceToCollision(Point, DistanceSq, ClosestPoint))
		{
			Distance = FMath::Sqrt(DistanceSq);
		}
		else
		{
			FBoxSphereBounds SphereBounds = ParentShape->CalcBounds(ParentShape->GetComponentTransform());
			if (ParentShape->IsA<USphereComponent>())
			{
				const FSphere& Sphere = SphereBounds.GetSphere();
				const FVector& Dist = Sphere.Center - Point;
				Distance = FMath::Max(0.0f, Dist.Size() - Sphere.W);
			}
			else // UBox or UCapsule shape (approx).
			{
				Distance = FMath::Sqrt(SphereBounds.GetBox().ComputeSquaredDistanceToPoint(Point));
			}
		}

		if (OutDistanceToPoint)
		{
			*OutDistanceToPoint = Distance;
		}

		return Distance >= 0.f && Distance <= SphereRadius;
	}
	if (OutDistanceToPoint != nullptr)
	{
		*OutDistanceToPoint = 0;
	}
	return true;
}

void UMeshPostProcessComponent::OnRegister()
{
	USceneComponent::OnRegister();
	GetWorld()->InsertPostProcessVolume(this);
}

void UMeshPostProcessComponent::OnUnregister()
{
	USceneComponent::OnUnregister();
	GetWorld()->RemovePostProcessVolume(this);
}

void UMeshPostProcessComponent::Serialize(FArchive& Ar)
{
	UObject::Serialize(Ar);

	if (Ar.IsLoading())
	{
#if WITH_EDITORONLY_DATA
		Settings.OnAfterLoad();
#endif
	}
}
