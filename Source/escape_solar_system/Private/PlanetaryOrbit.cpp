// Copyright 2020 H₂S. All Rights Reserved.

#include "PlanetaryOrbit.h"

APlanetaryOrbit::APlanetaryOrbit()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<UOrbitSceneComponent>(TEXT("RootComponent"));
}

void APlanetaryOrbit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CentralActor && RevolutionSpeed != 0)
	{
		SetActorLocation(CentralActor->GetActorLocation());
		GetRootComponent()->AddLocalRotation(FRotator(0.f, RevolutionSpeed * DeltaTime, 0.f));
	}
}

bool UOrbitSceneComponent::MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit, EMoveComponentFlags MoveFlags, ETeleportType Teleport)
{
	//SCOPE_CYCLE_COUNTER(STAT_MoveComponentSceneComponentTime);

	// static things can move before they are registered (e.g. immediately after streaming), but not after.
	if (!IsValid(this) || CheckStaticMobilityAndWarn(INVTEXT("move")))
	{
		if (OutHit)
		{
			*OutHit = FHitResult();
		}
		return false;
	}

	// Fill in optional output param. SceneComponent doesn't sweep, so this is just an empty result.
	if (OutHit)
	{
		*OutHit = FHitResult(1.f);
	}

	ConditionalUpdateComponentToWorld();

	// early out for zero case
	if (Delta.IsZero())
	{
		// Skip if no vector or rotation.
		if (NewRotation.Equals(GetComponentTransform().GetRotation(), SCENECOMPONENT_QUAT_TOLERANCE))
		{
			return true;
		}
	}

	// just teleport, sweep is supported for PrimitiveComponents. This will update child components as well.
	// 这整个函数都是拷贝自USceneComponent::MoveComponentImpl，除了下面这个调用
	const bool bMoved = InternalSetWorldLocationAndRotation(GetComponentLocation() + Delta, NewRotation, false, Teleport);

	// Only update overlaps if not deferring updates within a scope
	if (bMoved && !IsDeferringMovementUpdates())
	{
		// need to update overlap detection in case PrimitiveComponents are attached.
		UpdateOverlaps();
	}

	return true;
}

bool UOrbitSceneComponent::InternalSetWorldLocationAndRotation(FVector NewLocation, const FQuat& RotationQuat, bool bNoPhysics, ETeleportType Teleport)
{
	checkSlow(bComponentToWorldUpdated);
	FQuat NewRotationQuat(RotationQuat);

#if ENABLE_NAN_DIAGNOSTIC
	if (NewRotationQuat.ContainsNaN())
	{
		logOrEnsureNanError(TEXT("USceneComponent:InternalSetWorldLocationAndRotation found NaN in NewRotationQuat: %s"), *NewRotationQuat.ToString());
		NewRotationQuat = FQuat::Identity;
	}
#endif

	// If attached to something, transform into local space
	if (GetAttachParent() != nullptr)
	{
		FTransform const ParentToWorld = GetAttachParent()->GetSocketTransform(GetAttachSocketName());
		// in order to support mirroring, you'll have to use FTransform.GetrelativeTransform
		// because negative scale should flip the rotation
		if (FTransform::AnyHasNegativeScale(GetRelativeScale3D(), ParentToWorld.GetScale3D()))
		{
			FTransform const WorldTransform = FTransform(RotationQuat, NewLocation, GetRelativeScale3D() * ParentToWorld.GetScale3D());
			FTransform const RelativeTransform = WorldTransform.GetRelativeTransform(ParentToWorld);

			if (!IsUsingAbsoluteLocation())
			{
				NewLocation = RelativeTransform.GetLocation();
			}

			if (!IsUsingAbsoluteRotation())
			{
				NewRotationQuat = RelativeTransform.GetRotation();
			}
		}
		else
		{
			if (!IsUsingAbsoluteLocation())
			{
				NewLocation = ParentToWorld.InverseTransformPosition(NewLocation);
			}

			if (!IsUsingAbsoluteRotation())
			{
				// Quat multiplication works reverse way, make sure you do Parent(-1) * World = Local, not World*Parent(-) = Local (the way matrix does)
				NewRotationQuat = ParentToWorld.GetRotation().Inverse() * NewRotationQuat;
			}
		}
	}

	const FRotator NewRelativeRotation = GetRelativeRotationCache().QuatToRotator_ReadOnly(NewRotationQuat);
	bool bDiffLocation = !NewLocation.Equals(GetRelativeLocation());
	// 这整个函数都是拷贝自USceneComponent::InternalSetWorldLocationAndRotation，除了下面这个比较
	bool bDiffRotation = !NewRelativeRotation.Equals(GetRelativeRotation(), SMALL_NUMBER);
	if (bDiffLocation || bDiffRotation)
	{
		SetRelativeLocation_Direct(NewLocation);

		// Here it is important to compute the quaternion from the rotator and not the opposite.
		// In some cases, similar quaternions generate the same rotator, which create issues.
		// When the component is loaded, the rotator is used to generate the quaternion, which
		// is then used to compute the ComponentToWorld matrix. When running a blueprint script,  
		// it is required to generate that same ComponentToWorld otherwise the FComponentInstanceDataCache
		// might fail to apply to the relevant component. In order to have the exact same transform
		// we must enforce the quaternion to come from the rotator (as in load)
		if (bDiffRotation)
		{
			SetRelativeRotation_Direct(NewRelativeRotation);
			GetRelativeRotationCache().RotatorToQuat(NewRelativeRotation);
		}

#if ENABLE_NAN_DIAGNOSTIC
		if (GetRelativeRotation().ContainsNaN())
		{
			logOrEnsureNanError(TEXT("USceneComponent:InternalSetWorldLocationAndRotation found NaN in RelativeRotation: %s"), *GetRelativeRotation().ToString());
			SetRelativeRotation_Direct(FRotator::ZeroRotator);
		}
#endif
		UpdateComponentToWorld(SkipPhysicsToEnum(bNoPhysics), Teleport);

		// we need to call this even if this component itself is not navigation relevant
		if (IsRegistered() && bCanEverAffectNavigation)
		{
			PostUpdateNavigationData();
		}

		return true;
	}

	return false;
}
