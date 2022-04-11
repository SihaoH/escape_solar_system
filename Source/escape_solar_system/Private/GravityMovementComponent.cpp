// Copyright 2020 H₂S. All Rights Reserved.

#include "GravityMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameFramework/Character.h"
#include "GameFramework/NavMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AI/Navigation/AvoidanceManager.h"
#include "DrawDebugHelpers.h"
#include "AI/Navigation/PathFollowingAgentInterface.h"

// 如果 Abs(Normal.Z) <= 此阈值，则斜率是垂直的。 解决了有时垂直表面的法线角度略微偏离水平的精度问题。
const float VERTICAL_SLOPE_NORMAL_Z = 0.001f;

// 台阶垂直侧法线的最大Z值
const float MAX_STEP_SIDE_Z = 0.08f;

// Version that does not use inverse sqrt estimate, for higher precision.
FORCEINLINE FVector GetClampedToMaxSizePrecise(const FVector& V, float MaxSize)
{
	if (MaxSize < KINDA_SMALL_NUMBER)
	{
		return FVector::ZeroVector;
	}

	const float VSq = V.SizeSquared();
	if (VSq > FMath::Square(MaxSize))
	{
		return V * (MaxSize / FMath::Sqrt(VSq));
	}
	else
	{
		return V;
	}
}

UGravityMovementComponent::UGravityMovementComponent()
{
	PushForceFactor = 500.f;
}

void UGravityMovementComponent::PhysFlying(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		if (bCheatFlying && Acceleration.IsZero())
		{
			Velocity = FVector::ZeroVector;
		}
		const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction;
		CalcVelocity(deltaTime, Friction, true, GetMaxBrakingDeceleration());
	}

	ApplyRootMotionToVelocity(deltaTime);

	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.Time < 1.f)
	{
		const FVector GravDir = GravityDirection;
		const FVector VelDir = Velocity.GetSafeNormal();
		const float UpDown = GravDir | VelDir;

		bool bSteppedUp = false;
		if ((FMath::Abs(Hit.ImpactNormal.Z) < 0.2f) && (UpDown < 0.5f) && (UpDown > -0.2f) && CanStepUp(Hit))
		{
			float stepZ = UpdatedComponent->GetComponentLocation().Z;
			bSteppedUp = StepUp(GravDir, Adjusted * (1.f - Hit.Time), Hit);
			if (bSteppedUp)
			{
				OldLocation.Z = UpdatedComponent->GetComponentLocation().Z + (OldLocation.Z - stepZ);
			}
		}

		if (!bSteppedUp)
		{
			//adjust and try again
			HandleImpact(Hit, deltaTime, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}
	}

	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
}

float UGravityMovementComponent::BoostAirControl(float DeltaTime, float TickAirControl, const FVector& FallAcceleration)
{
	// Allow a burst of initial acceleration.
	//GetGravityDirection(true)
	if (AirControlBoostMultiplier > 0.f && FVector::VectorPlaneProject(Velocity, GravityDirection).SizeSquared() < FMath::Square(AirControlBoostVelocityThreshold))
	{
		TickAirControl = FMath::Min(1.f, AirControlBoostMultiplier * TickAirControl);
	}

	return TickAirControl;
}

void UGravityMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (!HasValidData())
	{
		return;
	}

	// Update collision settings if needed.
	if (MovementMode == MOVE_NavWalking)
	{
		// Reset cached nav location used by NavWalking
		CachedNavLocation = FNavLocation();

		SetGroundMovementMode(MovementMode);
		// Walking uses only XY velocity
		Velocity.Z = 0.f;
		SetNavWalkingPhysics(true);
	}
	else if (PreviousMovementMode == MOVE_NavWalking)
	{
		if (MovementMode == DefaultLandMovementMode || IsWalking())
		{
			const bool bSucceeded = TryToLeaveNavWalking();
			if (!bSucceeded)
			{
				return;
			}
		}
		else
		{
			SetNavWalkingPhysics(false);
		}
	}

	// React to changes in the movement mode.
	if (MovementMode == MOVE_Walking)
	{
		// Walking must be on a walkable floor, with a base.
		bCrouchMaintainsBaseLocation = true;
		SetGroundMovementMode(MovementMode);

		// Make sure we update our new floor/base on initial entry of the walking physics.
		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);
		UpdateComponentRotation();
		AdjustFloorHeight();
		SetBaseFromFloor(CurrentFloor);

		// Walking uses only horizontal velocity.
		MaintainHorizontalGroundVelocity();
	}
	else
	{
		CurrentFloor.Clear();
		bCrouchMaintainsBaseLocation = false;

		UpdateComponentRotation();

		if (MovementMode == MOVE_Falling)
		{
			Velocity += GetImpartedMovementBaseVelocity();
			CharacterOwner->Falling();
		}

		SetBase(NULL);

		if (MovementMode == MOVE_None)
		{
			// Kill velocity and clear queued up events.
			StopMovementKeepPathing();
			CharacterOwner->ResetJumpState();
			ClearAccumulatedForces();
		}
	}

	if (MovementMode == MOVE_Falling && PreviousMovementMode != MOVE_Falling)
	{
		IPathFollowingAgentInterface* PFAgent = GetPathFollowingAgent();
		if (PFAgent)
		{
			PFAgent->OnStartedFalling();
		}
	}

	CharacterOwner->OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UGravityMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
	UpdateComponentRotation();
}

void UGravityMovementComponent::HandleImpact(const FHitResult& Impact, float TimeSlice, const FVector& MoveDelta)
{
	//SCOPE_CYCLE_COUNTER(STAT_CharHandleImpact);

	if (CharacterOwner)
	{
		CharacterOwner->MoveBlockedBy(Impact);
	}

	IPathFollowingAgentInterface* PFAgent = GetPathFollowingAgent();
	if (PFAgent)
	{
		// Also notify path following!
		PFAgent->OnMoveBlockedBy(Impact);
	}

	APawn* OtherPawn = Cast<APawn>(Impact.GetActor());
	if (OtherPawn)
	{
		NotifyBumpedPawn(OtherPawn);
	}

	if (bEnablePhysicsInteraction)
	{
		const FVector ForceAccel = Acceleration + (IsFalling() ? GetGravity() : FVector::ZeroVector);
		ApplyImpactPhysicsForces(Impact, ForceAccel, Velocity);
	}
}

void UGravityMovementComponent::MaintainHorizontalGroundVelocity()
{
	if (!Velocity.IsZero())
	{
		if (bMaintainHorizontalGroundVelocity)
		{
			// Just remove the vertical component.
			Velocity = FVector::VectorPlaneProject(Velocity, GetUpVector());
		}
		else
		{
			// Project the vector and maintain its original magnitude.
			Velocity = FVector::VectorPlaneProject(Velocity, GetUpVector()).GetSafeNormal() * Velocity.Size();
		}
	}
}

float UGravityMovementComponent::SlideAlongSurface(const FVector& Delta, float Time, const FVector& InNormal, FHitResult& Hit, bool bHandleImpact)
{
	if (!Hit.bBlockingHit)
	{
		return 0.f;
	}

	FVector Normal(InNormal);
	if (IsMovingOnGround())
	{
		const FVector CapsuleUp = GetUpVector();
		const float Dot = Normal | CapsuleUp;

		// We don't want to be pushed up an unwalkable surface.
		if (Dot > 0.f)
		{
			if (!IsWalkable(Hit))
			{
				Normal = FVector::VectorPlaneProject(Normal, CapsuleUp).GetSafeNormal();
			}
		}
		else if (Dot < -KINDA_SMALL_NUMBER)
		{
			// Don't push down into the floor when the impact is on the upper portion of the capsule.
			if (CurrentFloor.FloorDist < MIN_FLOOR_DIST && CurrentFloor.bBlockingHit)
			{
				const FVector FloorNormal = CurrentFloor.HitResult.Normal;
				const bool bFloorOpposedToMovement = (Delta | FloorNormal) < 0.f && (FloorNormal | CapsuleUp) < 1.f - DELTA;
				if (bFloorOpposedToMovement)
				{
					Normal = FloorNormal;
				}

				Normal = FVector::VectorPlaneProject(Normal, CapsuleUp).GetSafeNormal();
			}
		}
	}

	return Super::SlideAlongSurface(Delta, Time, Normal, Hit, bHandleImpact);
}

void UGravityMovementComponent::SetPostLandedPhysics(const FHitResult& Hit)
{
	if (CharacterOwner)
	{
		if (GetPhysicsVolume()->bWaterVolume && CanEverSwim())
		{
			SetMovementMode(MOVE_Swimming);
		}
		else
		{
			const FVector PreImpactAccel = Acceleration + (IsFalling() ? GetGravity() : FVector::ZeroVector);
			const FVector PreImpactVelocity = Velocity;
			
			if (DefaultLandMovementMode == MOVE_Walking ||
				DefaultLandMovementMode == MOVE_NavWalking ||
				DefaultLandMovementMode == MOVE_Falling)
			{
				SetMovementMode(GetGroundMovementMode());
			}
			else
			{
				SetDefaultMovementMode();
			}

			ApplyImpactPhysicsForces(Hit, PreImpactAccel, PreImpactVelocity);
		}
	}
}

void UGravityMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	//SCOPE_CYCLE_COUNTER(STAT_CharPhysWalking);
	CSV_SCOPED_TIMING_STAT_EXCLUSIVE(CharPhysWalking);

	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	if (!UpdatedComponent->IsQueryCollisionEnabled())
	{
		SetMovementMode(MOVE_Walking);
		return;
	}

	checkf(!Velocity.ContainsNaN(), TEXT("PhysWalking: Velocity contains NaN before Iteration (%s)\n%s"), *GetPathNameSafe(this), *Velocity.ToString());

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float RemainingTime = deltaTime;

	// Perform the move.
	while ((RemainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float TimeTick = GetSimulationTimeStep(RemainingTime, Iterations);
		RemainingTime -= TimeTick;

		// Save current values.
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		RestorePreAdditiveRootMotionVelocity();

		// Acceleration is already horizontal; ensure velocity is also horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;

		// Apply acceleration.
		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			CalcVelocity(TimeTick, GroundFriction, false, GetMaxBrakingDeceleration());
		}

		ApplyRootMotionToVelocity(TimeTick);
		checkf(!Velocity.ContainsNaN(), TEXT("PhysWalking: Velocity contains NaN after CalcVelocity (%s: %s)\n%s"), *GetPathNameSafe(this), *GetPathNameSafe(GetOuter()), *Velocity.ToString());

		if (IsFalling())
		{
			// Root motion could have put us into Falling.
			// No movement has taken place this movement tick so we pass on full time/past iteration count
			StartNewPhysics(RemainingTime + TimeTick, Iterations - 1);
			return;
		}

		// Compute move parameters.
		const FVector MoveVelocity = Velocity;
		const FVector Delta = TimeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;

		if (bZeroDelta)
		{
			RemainingTime = 0.f;
		}
		else
		{
			// Try to move forward.
			MoveAlongFloor(MoveVelocity, TimeTick, &StepDownResult);

			if (IsFalling())
			{
				// Pawn decided to jump up.
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = FVector::VectorPlaneProject(CharacterOwner->GetActorLocation() - OldLocation, GetUpVector()).Size();
					RemainingTime += TimeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}

				StartNewPhysics(RemainingTime, Iterations);
				return;
			}
			else if (IsSwimming()) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, TimeTick, RemainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}

		// Check for ledges here.
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// Calculate possible alternate movement.
			const FVector GravDir = GravityDirection;
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
			if (!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction.
				Velocity = NewDelta / TimeTick;
				RemainingTime += TimeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump.
				// @todo collision: only thing that can be problem is that OldBase has world collision on.
				bool bMustJump = bZeroDelta || OldBase == NULL || (!OldBase->IsCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, RemainingTime, TimeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				// revert this move.
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				RemainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check.
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					//CharacterOwner->OnWalkingOffLedge();
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, RemainingTime, TimeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && RemainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration.
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + GetUpVector() * MAX_FLOOR_DIST;
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, CharacterOwner->GetActorRotation());
				bForceNextFloorCheck = true;
			}

			// Check if just entered water.
			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, TimeTick, RemainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || OldBase == NULL || (!OldBase->IsCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, RemainingTime, TimeTick, Iterations, bMustJump))
				{
					return;
				}

				bCheckedFall = true;
			}
		}

		// Allow overlap events and such to change physics state and velocity.
		if (IsMovingOnGround())
		{
			// Make velocity reflect actual move.
			if (!bJustTeleported && !HasAnimRootMotion() && TimeTick >= MIN_TICK_TIME)
			{
				Velocity = (CharacterOwner->GetActorLocation() - OldLocation) / TimeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (CharacterOwner->GetActorLocation() == OldLocation)
		{
			RemainingTime = 0.f;
			break;
		}
	}

	if (IsMovingOnGround())
	{
		MaintainHorizontalGroundVelocity();
	}
}

FVector UGravityMovementComponent::ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const
{
	const FVector FloorNormal = RampHit.ImpactNormal;
	const FVector ContactNormal = GetUpVector();

	// THIS IS THE PROBLEM
	bool bb = true; // = FMath::Abs(Delta | FloorNormal) > THRESH_NORMALS_ARE_ORTHOGONAL;
	bool cc = IsWalkable(RampHit);
	if (!bHitFromLineTrace && bb && cc)
	{
		// Compute a vector that moves parallel to the surface, by projecting the horizontal movement direction onto the ramp.
		// We can't just project Delta onto the plane defined by FloorNormal because the direction changes on spherical geometry.
		const FVector DeltaNormal = Delta.GetSafeNormal();
		FVector NewDelta = FQuat(ContactNormal ^ DeltaNormal, FMath::Acos(FloorNormal | ContactNormal)).RotateVector(Delta);

		if (bShowDebugLines)
		{
			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + NewDelta * 30, FColor::Red, true, 10.f);
		}

		if (bMaintainHorizontalGroundVelocity)
		{
			const FVector NewDeltaNormal = NewDelta.GetSafeNormal();
			NewDelta = NewDeltaNormal * (Delta.Size() / (DeltaNormal | NewDeltaNormal));
		}

		return NewDelta;
	}

	return Delta;
}

void UGravityMovementComponent::MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult /*= NULL*/)
{
	if (!CurrentFloor.IsWalkableFloor())
	{
		return;
	}

	const FVector Delta = FVector::VectorPlaneProject(InVelocity, GetUpVector()) * DeltaSeconds;
	FHitResult Hit(1.f);
	FVector RampVector = ComputeGroundMovementDelta(Delta, CurrentFloor.HitResult, CurrentFloor.bLineTrace);

	if (bShowDebugLines)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (RampVector * 20), FColor::Blue, true, 10.f, 10, 6.f);
	}

	SafeMoveUpdatedComponent(RampVector, CharacterOwner->GetActorRotation(), true, Hit);
	float LastMoveTimeSlice = DeltaSeconds;

	if (Hit.bStartPenetrating)
	{
		// Allow this hit to be used as an impact we can deflect off, otherwise we do nothing the rest of the update and appear to hitch.
		HandleImpact(Hit);
		SlideAlongSurface(Delta, 1.f, Hit.Normal, Hit, true);

		if (Hit.bStartPenetrating)
		{
			OnCharacterStuckInGeometry(&Hit);
		}
	}
	else if (Hit.IsValidBlockingHit())
	{
		// We impacted something (most likely another ramp, but possibly a barrier).
		float PercentTimeApplied = Hit.Time;
		if (Hit.Time > 0.f && (Hit.Normal | GetUpVector()) > KINDA_SMALL_NUMBER && IsWalkable(Hit))
		{
			// Another walkable ramp.
			const float InitialPercentRemaining = 1.f - PercentTimeApplied;
			RampVector = ComputeGroundMovementDelta(Delta * InitialPercentRemaining, Hit, false);
			LastMoveTimeSlice = InitialPercentRemaining * LastMoveTimeSlice;
			SafeMoveUpdatedComponent(RampVector, CharacterOwner->GetActorRotation(), true, Hit);

			const float SecondHitPercent = Hit.Time * InitialPercentRemaining;
			PercentTimeApplied = FMath::Clamp(PercentTimeApplied + SecondHitPercent, 0.f, 1.f);
		}

		if (Hit.IsValidBlockingHit())
		{
			if (CanStepUp(Hit) || (CharacterOwner->GetMovementBase() != NULL && CharacterOwner->GetMovementBase()->GetOwner() == Hit.GetActor()))
			{
				// hit a barrier, try to step up.
				const FVector PreStepUpLocation = UpdatedComponent->GetComponentLocation();
				const FVector GravDir = GravityDirection;
				if (!StepUp(GravDir, Delta * (1.f - PercentTimeApplied), Hit, OutStepDownResult))
				{
					//UE_LOG(LogCharacterMovement, Verbose, TEXT("- StepUp (ImpactNormal %s, Normal %s"), *Hit.ImpactNormal.ToString(), *Hit.Normal.ToString());
					HandleImpact(Hit, LastMoveTimeSlice, RampVector);
					SlideAlongSurface(Delta, 1.f - PercentTimeApplied, Hit.Normal, Hit, true);
				}
				else
				{
					//UE_LOG(LogCharacterMovement, Verbose, TEXT("+ StepUp (ImpactNormal %s, Normal %s"), *Hit.ImpactNormal.ToString(), *Hit.Normal.ToString());
					if (!bMaintainHorizontalGroundVelocity)
					{
						// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments. Only consider horizontal movement.
						bJustTeleported = true;
						const float StepUpTimeSlice = (1.f - PercentTimeApplied) * DeltaSeconds;
						if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && StepUpTimeSlice >= KINDA_SMALL_NUMBER)
						{
							Velocity = (UpdatedComponent->GetComponentLocation() - PreStepUpLocation) / StepUpTimeSlice;
						}
					}
				}
			}
			else if (Hit.Component.IsValid() && !Hit.Component.Get()->CanCharacterStepUp(CharacterOwner))
			{
				HandleImpact(Hit, LastMoveTimeSlice, RampVector);
				SlideAlongSurface(Delta, 1.f - PercentTimeApplied, Hit.Normal, Hit, true);
			}
		}
	}
}

void UGravityMovementComponent::SimulateMovement(float DeltaSeconds)
{
	if (!HasValidData() || UpdatedComponent->Mobility != EComponentMobility::Movable || UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	const bool bIsSimulatedProxy = (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy);

	const FRepMovement& ConstRepMovement = CharacterOwner->GetReplicatedMovement();

	// Workaround for replication not being updated initially.
	if (bIsSimulatedProxy && 
		ConstRepMovement.Location.IsZero() &&
		ConstRepMovement.Rotation.IsZero() &&
		ConstRepMovement.LinearVelocity.IsZero())
	{
		return;
	}

	// If base is not resolved on the client, we should not try to simulate at all.
	if (CharacterOwner->GetReplicatedBasedMovement().IsBaseUnresolved())
	{
		//UE_LOG(LogCharacterMovement, Verbose, TEXT("Base for simulated character '%s' is not resolved on client, skipping SimulateMovement"), *CharacterOwner->GetName());
		return;
	}

	UpdateComponentRotation(); //needed?

	FVector OldVelocity;
	FVector OldLocation;

	// Scoped updates can improve performance of multiple MoveComponent calls.
	{
		FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);

		bool bHandledNetUpdate = false;
		if (bIsSimulatedProxy)
		{
			// Handle network changes.
			if (bNetworkUpdateReceived)
			{
				bNetworkUpdateReceived = false;
				bHandledNetUpdate = true;
				//UE_LOG(LogCharacterMovement, Verbose, TEXT("Proxy %s received net update"), *CharacterOwner->GetName());
				if (bNetworkMovementModeChanged)
				{
					ApplyNetworkMovementMode(CharacterOwner->GetReplicatedMovementMode());
					bNetworkMovementModeChanged = false;
				}
				else if (bJustTeleported || bForceNextFloorCheck)
				{
					// Make sure floor is current. We will continue using the replicated base, if there was one.
					bJustTeleported = false;
					UpdateFloorFromAdjustment();
				}
			}
			else if (bForceNextFloorCheck)
			{
				UpdateFloorFromAdjustment();
			}
		}

		UpdateCharacterStateBeforeMovement(DeltaSeconds);

		if (MovementMode != MOVE_None)
		{
			HandlePendingLaunch();
		}
		ClearAccumulatedForces();

		if (MovementMode == MOVE_None)
		{
			return;
		}

		const bool bSimGravityDisabled = (bIsSimulatedProxy && CharacterOwner->bSimGravityDisabled);
		const bool bZeroReplicatedGroundVelocity = (bIsSimulatedProxy && IsMovingOnGround() && ConstRepMovement.LinearVelocity.IsZero());

		// bSimGravityDisabled means velocity was zero when replicated and we were stuck in something. Avoid external changes in velocity as well.
		// Being in ground movement with zero velocity, we cannot simulate proxy velocities safely because we might not get any further updates from the server.
		if (bSimGravityDisabled || bZeroReplicatedGroundVelocity)
		{
			Velocity = FVector::ZeroVector;
		}

		MaybeUpdateBasedMovement(DeltaSeconds);

		// Simulated pawns predict location.
		OldVelocity = Velocity;
		OldLocation = UpdatedComponent->GetComponentLocation();

		UpdateProxyAcceleration();

		if (!bHandledNetUpdate || !bNetworkSkipProxyPredictionOnNetUpdate)
		{
			//UE_LOG(LogCharacterMovement, Verbose, TEXT("Proxy %s simulating movement"), *GetNameSafe(CharacterOwner));
			FStepDownResult StepDownResult;
			MoveSmooth(Velocity, DeltaSeconds, &StepDownResult);

			if (IsMovingOnGround() || MovementMode == MOVE_Falling)
			{
				const FVector Gravity = GetGravity();

				if (StepDownResult.bComputedFloor)
				{
					CurrentFloor = StepDownResult.FloorResult;
				}
				else
				{
					if (!Gravity.IsZero() && (Velocity | Gravity) >= 0.f)
					{
						FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, Velocity.IsZero(), NULL);
					}
					else
					{
						CurrentFloor.Clear();
					}
				}

				if (!CurrentFloor.IsWalkableFloor())
				{
					// No floor, must fall.
					if (!bSimGravityDisabled)
					{
						Velocity = NewFallVelocity(Velocity, Gravity, DeltaSeconds);
					}
					SetMovementMode(MOVE_Falling);
				}
				else
				{
					// Walkable floor.
					if (IsMovingOnGround())
					{
						AdjustFloorHeight();
						SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
					}
					else if (MovementMode == MOVE_Falling)
					{
						if (CurrentFloor.FloorDist <= MIN_FLOOR_DIST || (bSimGravityDisabled && CurrentFloor.FloorDist <= MAX_FLOOR_DIST))
						{
							// Landed.
							SetPostLandedPhysics(CurrentFloor.HitResult);
						}
						else
						{
							// Continue falling.
							if (!bSimGravityDisabled)
							{
								Velocity = NewFallVelocity(Velocity, Gravity, DeltaSeconds);
							}
							CurrentFloor.Clear();
						}
					}
				}
			}
		}
		else
		{
			//UE_LOG(LogCharacterMovement, Verbose, TEXT("Proxy %s SKIPPING simulate movement"), *GetNameSafe(CharacterOwner));
		}

		UpdateCharacterStateAfterMovement(DeltaSeconds);

		// consume path following requested velocity
		bHasRequestedVelocity = false;

		OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	} // End scoped movement update.

	// Call custom post-movement events. These happen after the scoped movement completes in case the events want to use the current state of overlaps etc.
	CallMovementUpdateDelegate(DeltaSeconds, OldLocation, OldVelocity);

	SaveBaseLocation();
	UpdateComponentVelocity();
	bJustTeleported = false;

	LastUpdateLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
	LastUpdateRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;
	LastUpdateVelocity = Velocity;
}

FVector UGravityMovementComponent::ConstrainInputAcceleration(const FVector& InputAcceleration) const
{
	// Walking or falling pawns ignore up/down sliding.
	if (IsMovingOnGround() || IsFalling())
	{
		return FVector::VectorPlaneProject(InputAcceleration, GetUpVector());
	}

	return InputAcceleration;
}

FVector UGravityMovementComponent::ScaleInputAcceleration(const FVector& InputAcceleration) const
{
	return GetMaxAcceleration() * GetClampedToMaxSizePrecise(InputAcceleration, 1.f);
}

FVector UGravityMovementComponent::CalcAnimRootMotionVelocity(const FVector& RootMotionDeltaMove, float DeltaTime, const FVector& CurrentVelocity) const
{
	FVector RootMotionVelocity = RootMotionDeltaMove / DeltaTime;

	// Do not override vertical velocity if in falling physics, we want to keep the effect of gravity.
	if (IsFalling())
	{
		//GetGravityDirection(true);
		RootMotionVelocity = FVector::VectorPlaneProject(RootMotionVelocity, GravityDirection) + GravityDirection * (Velocity | GravityDirection);
	}

	return RootMotionVelocity;
}

void UGravityMovementComponent::StartFalling(int32 Iterations, float remainingTime, float timeTick, const FVector& Delta, const FVector& subLoc)
{
	const float DesiredDist = Delta.Size();
	const float ActualDist = (UpdatedComponent->GetComponentLocation() - subLoc).Size2D();
	remainingTime = (DesiredDist < KINDA_SMALL_NUMBER)
		? 0.f
		: remainingTime + timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));

	if (!GravityDirection.IsZero())
	{
		Velocity = FVector::VectorPlaneProject(Velocity, GravityDirection);
	}

	if (IsMovingOnGround())
	{
		// This is to catch cases where the first frame of PIE is executed, and the
		// level is not yet visible. In those cases, the player will fall out of the
		// world... So, don't set MOVE_Falling straight away.
		if (!GIsEditor || (GetWorld()->HasBegunPlay() && GetWorld()->GetTimeSeconds() >= 1.f))
		{
			SetMovementMode(MOVE_Falling); // Default behavior if script didn't change physics.
		}
		else
		{
			// Make sure that the floor check code continues processing during this delay.
			bForceNextFloorCheck = true;
		}
	}
	StartNewPhysics(remainingTime, Iterations);
}

void UGravityMovementComponent::PhysFalling(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	FVector FallAcceleration = GetFallingLateralAcceleration(deltaTime);
	const bool bHasAirControl = FallAcceleration.SizeSquared() > 0.f;

	float RemainingTime = deltaTime;
	while (RemainingTime >= MIN_TICK_TIME && Iterations < MaxSimulationIterations)
	{
		Iterations++;
		float TimeTick = GetSimulationTimeStep(RemainingTime, Iterations);
		RemainingTime -= TimeTick;

		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FQuat PawnRotation = UpdatedComponent->GetComponentQuat();
		bJustTeleported = false;

		RestorePreAdditiveRootMotionVelocity();
		
		FVector OldVelocity = Velocity;

		// Apply input.
		const float MaxDecel = GetMaxBrakingDeceleration();
		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			const FVector OldVelocityZ = GravityDirection * (Velocity | GravityDirection);
			// Compute Velocity.
			{
				// Acceleration = FallAcceleration for CalcVelocity(), but we restore it after using it.
				TGuardValue<FVector> RestoreAcceleration(Acceleration, FallAcceleration);

				Velocity = FVector::VectorPlaneProject(Velocity, GravityDirection);
				CalcVelocity(TimeTick, FallingLateralFriction, false, BrakingDecelerationFalling);
				Velocity = FVector::VectorPlaneProject(Velocity, GravityDirection) + OldVelocityZ;
			}
		}

		// Compute current gravity
		const FVector Gravity = GetGravity();
		float GravityTime = TimeTick;

		// If jump is providing force, gravity may be affected.
		bool bEndingJumpForce = false;
		if (CharacterOwner->JumpForceTimeRemaining > 0.f)
		{
			// Consume some of the force time. Only the remaining time (if any) is affected by gravity when bApplyGravityWhileJumping=false.
			const float JumpForceTime = FMath::Min(CharacterOwner->JumpForceTimeRemaining, TimeTick);
			GravityTime = bApplyGravityWhileJumping ? TimeTick : FMath::Max(0.f, TimeTick - JumpForceTime);

			// Update Character state
			CharacterOwner->JumpForceTimeRemaining -= JumpForceTime;
			if (CharacterOwner->JumpForceTimeRemaining <= 0.f)
			{
				CharacterOwner->ResetJumpState();
				bEndingJumpForce = true;
			}
		}

		// Apply gravity.
		Velocity = NewFallVelocity(Velocity, Gravity, TimeTick);
		ApplyRootMotionToVelocity(TimeTick);

		if (bNotifyApex && CharacterOwner->Controller && ((Velocity | GravityDirection) * -1.f) <= 0.f)
		{
			// Just passed jump apex since now going down.
			bNotifyApex = false;
			NotifyJumpApex();
		}

		// Compute change in position (using midpoint integration method).
		FVector Adjusted = 0.5f * (OldVelocity + Velocity) * TimeTick;

		// Special handling if ending the jump force where we didn't apply gravity during the jump.
		if (bEndingJumpForce && !bApplyGravityWhileJumping)
		{
			// We had a portion of the time at constant speed then a portion with acceleration due to gravity.
			// Account for that here with a more correct change in position.
			const float NonGravityTime = FMath::Max(0.f, TimeTick - GravityTime);
			Adjusted = (OldVelocity * NonGravityTime) + (0.5f * (OldVelocity + Velocity) * GravityTime);
		}

		// Move
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Adjusted, PawnRotation, true, Hit);

		if (!HasValidData())
		{
			return;
		}

		float LastMoveTimeSlice = TimeTick;
		float SubTimeTickRemaining = TimeTick * (1.f - Hit.Time);

		if (IsSwimming()) //just entered water
		{
			// Just entered water.
			RemainingTime += SubTimeTickRemaining;
			StartSwimming(OldLocation, OldVelocity, TimeTick, RemainingTime, Iterations);
			return;
		}
		else if (Hit.bBlockingHit)
		{
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
			{
				RemainingTime += SubTimeTickRemaining;
				ProcessLanded(Hit, RemainingTime, Iterations);
				return;
			}
			else
			{
				// Compute impact deflection based on final velocity, not integration step.
				// This allows us to compute a new velocity from the deflected vector, and ensures the full gravity effect is included in the slide result.
				Adjusted = Velocity * TimeTick;

				// See if we can convert a normally invalid landing spot (based on the hit result) to a usable one.
				if (!Hit.bStartPenetrating && ShouldCheckForValidLandingSpot(TimeTick, Adjusted, Hit))
				{
					const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
					FFindFloorResult FloorResult;
					FindFloor(PawnLocation, FloorResult, false);
					if (FloorResult.IsWalkableFloor() && IsValidLandingSpot(PawnLocation, FloorResult.HitResult))
					{
						RemainingTime += SubTimeTickRemaining;
						ProcessLanded(FloorResult.HitResult, RemainingTime, Iterations);
						return;
					}
				}

				HandleImpact(Hit, LastMoveTimeSlice, Adjusted);

				// If we've changed physics mode, abort.
				if (!HasValidData() || !IsFalling())
				{
					return;
				}

				// Limit air control based on what we hit.
				// We moved to the impact point using air control, but may want to deflect from there based on a limited air control acceleration.
				FVector VelocityNoAirControl = OldVelocity;
				FVector AirControlAccel = Acceleration;
				if (bHasAirControl)
				{
					const FVector AirControlDeltaV = LimitAirControl(LastMoveTimeSlice, AirControlAccel, Hit, false) * LastMoveTimeSlice;
					Adjusted = (VelocityNoAirControl + AirControlDeltaV) * LastMoveTimeSlice;
				}

				const FVector OldHitNormal = Hit.Normal;
				const FVector OldHitImpactNormal = Hit.ImpactNormal;
				FVector Delta = ComputeSlideVector(Adjusted, 1.f - Hit.Time, OldHitNormal, Hit);

				// Compute velocity after deflection (only gravity component for RootMotion).
				if (SubTimeTickRemaining > KINDA_SMALL_NUMBER && !bJustTeleported)
				{
					const FVector NewVelocity = (Delta / SubTimeTickRemaining);
					if (HasAnimRootMotion())
					{
						Velocity = FVector::VectorPlaneProject(Velocity, GravityDirection) + GravityDirection * (NewVelocity | GravityDirection);
					}
					else
					{
						Velocity = NewVelocity;
					}
				}

				if (SubTimeTickRemaining > KINDA_SMALL_NUMBER && (Delta | Adjusted) > 0.f)
				{
					// Move in deflected direction.
					SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);

					if (Hit.bBlockingHit)
					{
						// Hit second wall.
						LastMoveTimeSlice = SubTimeTickRemaining;
						SubTimeTickRemaining *= (1.f - Hit.Time);

						if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
						{
							RemainingTime += SubTimeTickRemaining;
							ProcessLanded(Hit, RemainingTime, Iterations);
							return;
						}

						HandleImpact(Hit, LastMoveTimeSlice, Delta);

						// If we've changed physics mode, abort.
						if (!HasValidData() || !IsFalling())
						{
							return;
						}

						// Act as if there was no air control on the last move when computing new deflection.
						if (bHasAirControl && (Hit.Normal | GravityDirection) < -VERTICAL_SLOPE_NORMAL_Z)
						{
							const FVector LastMoveNoAirControl = VelocityNoAirControl * LastMoveTimeSlice;
							Delta = ComputeSlideVector(LastMoveNoAirControl, 1.f, OldHitNormal, Hit);
						}

						FVector PreTwoWallDelta = Delta;
						TwoWallAdjust(Delta, Hit, OldHitNormal);

						// Limit air control, but allow a slide along the second wall.
						if (bHasAirControl)
						{
							const bool bCheckLandingSpot = false; // we already checked above.
							const FVector AirControlDeltaV = LimitAirControl(SubTimeTickRemaining, AirControlAccel, Hit, bCheckLandingSpot) * SubTimeTickRemaining;

							// Only allow if not back in to first wall
							if (FVector::DotProduct(AirControlDeltaV, OldHitNormal) > 0.f)
							{
								Delta += (AirControlDeltaV * SubTimeTickRemaining);
							}
						}

						// Compute velocity after deflection (only gravity component for RootMotion).
						if (SubTimeTickRemaining > KINDA_SMALL_NUMBER && !bJustTeleported)
						{
							const FVector NewVelocity = (Delta / SubTimeTickRemaining);

							if (HasAnimRootMotion())
							{
								Velocity = FVector::VectorPlaneProject(Velocity, GravityDirection) + GravityDirection * (NewVelocity | GravityDirection);
							}
							else
							{
								Velocity = NewVelocity;
							}
						}

						// bDitch=true means that pawn is straddling two slopes, neither of which he can stand on.
						bool bDitch = ((OldHitImpactNormal | GravityDirection) < 0.f && (Hit.ImpactNormal | GravityDirection) < 0.f &&
							FMath::Abs(Delta | GravityDirection) <= KINDA_SMALL_NUMBER && (Hit.ImpactNormal | OldHitImpactNormal) < 0.f);

						SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);
						if (Hit.Time == 0.f)
						{
							// If we are stuck then try to side step.
							FVector SideDelta = FVector::VectorPlaneProject(OldHitNormal + Hit.ImpactNormal, GravityDirection).GetSafeNormal();
							if (SideDelta.IsNearlyZero())
							{
								SideDelta = GravityDirection ^ (FVector::VectorPlaneProject(OldHitNormal, GravityDirection).GetSafeNormal());
							}

							SafeMoveUpdatedComponent(SideDelta, PawnRotation, true, Hit);
						}

						if (bDitch || IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit) || Hit.Time == 0.f)
						{
							RemainingTime = 0.f;
							ProcessLanded(Hit, RemainingTime, Iterations);
							return;
						}
						else if (GetPerchRadiusThreshold() > 0.f && Hit.Time == 1.f && (OldHitImpactNormal | GravityDirection) <= -GetWalkableFloorZ())
						{
							// We might be in a virtual 'ditch' within our perch radius. This is rare.
							const FVector PawnLocation = CharacterOwner->GetActorLocation();
							const float ZMovedDist = FMath::Abs((PawnLocation - OldLocation) | GravityDirection);
							const float MovedDist2DSq = (FVector::VectorPlaneProject(PawnLocation - OldLocation, GravityDirection)).SizeSquared();

							if (ZMovedDist <= 0.2f * TimeTick && MovedDist2DSq <= 4.f * TimeTick)
							{
								Velocity.X += 0.25f * GetMaxSpeed() * (FMath::FRand() - 0.5f);
								Velocity.Y += 0.25f * GetMaxSpeed() * (FMath::FRand() - 0.5f);
								Velocity.Z += 0.25f * GetMaxSpeed() * (FMath::FRand() - 0.5f);
								Velocity = FVector::VectorPlaneProject(Velocity, GravityDirection) + GravityDirection * (FMath::Max<float>(JumpZVelocity * 0.25f, 1.f) * -1.f);
								Delta = Velocity * TimeTick;
								SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);
							}
						}
					}
				}
			}
		}

		if ((FVector::VectorPlaneProject(Velocity, GravityDirection)).SizeSquared() <= KINDA_SMALL_NUMBER * 10.f)
		{
			Velocity = GravityDirection * (Velocity | GravityDirection);
		}
	}

	return;
}

FVector UGravityMovementComponent::GetFallingLateralAcceleration(float DeltaTime)
{
	FVector FallAcceleration = FVector::VectorPlaneProject(Acceleration, GravityDirection);

	// bound acceleration, falling object has minimal ability to impact acceleration
	if (!HasAnimRootMotion() && FallAcceleration.SizeSquared2D() > 0.f)
	{
		float FallAirControl = AirControl;

		// Allow a burst of initial acceleration.
		if (FallAirControl != 0.f && AirControlBoostMultiplier > 0.f &&
			FVector::VectorPlaneProject(Velocity, GravityDirection).SizeSquared() < FMath::Square(AirControlBoostVelocityThreshold))
		{
			FallAirControl = FMath::Min(1.f, AirControlBoostMultiplier * FallAirControl);
		}

		FallAcceleration *= FallAirControl;
		FallAcceleration = FallAcceleration.GetClampedToMaxSize(GetMaxAcceleration());
	}

	return FallAcceleration;
}

void UGravityMovementComponent::UpdateBasedMovement(float DeltaSeconds)
{
	if (!HasValidData())
	{
		return;
	}

	const UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
	if (!MovementBaseUtility::UseRelativeLocation(MovementBase))
	{
		return;
	}

	if (!IsValid(MovementBase) || !IsValid(MovementBase->GetOwner()))
	{
		SetBase(NULL);
		return;
	}

	// Ignore collision with bases during these movements.
	TGuardValue<EMoveComponentFlags> ScopedFlagRestore(MoveComponentFlags, MoveComponentFlags | MOVECOMP_IgnoreBases);

	FQuat DeltaQuat = FQuat::Identity;
	FVector DeltaPosition = FVector::ZeroVector;

	FQuat NewBaseQuat;
	FVector NewBaseLocation;
	if (!MovementBaseUtility::GetMovementBaseTransform(MovementBase, CharacterOwner->GetBasedMovement().BoneName, NewBaseLocation, NewBaseQuat))
	{
		return;
	}

	// Find change in rotation
	const bool bRotationChanged = !OldBaseQuat.Equals(NewBaseQuat, 1e-8f);
	if (bRotationChanged)
	{
		DeltaQuat = NewBaseQuat * OldBaseQuat.Inverse();
	}

	// only if base moved
	if (bRotationChanged || (OldBaseLocation != NewBaseLocation))
	{
		// Calculate new transform matrix of base actor (ignoring scale).
		const FQuatRotationTranslationMatrix OldLocalToWorld(OldBaseQuat, OldBaseLocation);
		const FQuatRotationTranslationMatrix NewLocalToWorld(NewBaseQuat, NewBaseLocation);

		FQuat FinalQuat = UpdatedComponent->GetComponentQuat();

		if (bRotationChanged && !bIgnoreBaseRotation)
		{
			// Apply change in rotation and pipe through FaceRotation to maintain axis restrictions
			const FQuat PawnOldQuat = UpdatedComponent->GetComponentQuat();
			const FQuat TargetQuat = DeltaQuat * FinalQuat;
			FRotator TargetRotator(TargetQuat);
			CharacterOwner->FaceRotation(TargetRotator, 0.f);
			FinalQuat = UpdatedComponent->GetComponentQuat();

			if (PawnOldQuat.Equals(FinalQuat, 1e-6f))
			{
				// Nothing changed. This means we probably are using another rotation mechanism (bOrientToMovement etc). We should still follow the base object.
				// @todo: This assumes only Yaw is used, currently a valid assumption. This is the only reason FaceRotation() is used above really, aside from being a virtual hook.
				if (bOrientRotationToMovement || (bUseControllerDesiredRotation && CharacterOwner->Controller))
				{
					TargetRotator.Pitch = 0.f;
					TargetRotator.Roll = 0.f;
					MoveUpdatedComponent(FVector::ZeroVector, TargetRotator, false);
					FinalQuat = UpdatedComponent->GetComponentQuat();
				}
			}

			// Pipe through ControlRotation, to affect camera.
			if (CharacterOwner->Controller)
			{
				const FQuat PawnDeltaRotation = FinalQuat * PawnOldQuat.Inverse();
				FRotator FinalRotation = FinalQuat.Rotator();
				UpdateBasedRotation(FinalRotation, PawnDeltaRotation.Rotator());
				FinalQuat = UpdatedComponent->GetComponentQuat();
			}
		}

		// We need to offset the base of the character here, not its origin, so offset by half height
		float HalfHeight, Radius;
		CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);

		FVector const BaseOffset = GetUpVector() * HalfHeight;
		FVector const LocalBasePos = OldLocalToWorld.InverseTransformPosition(UpdatedComponent->GetComponentLocation() - BaseOffset);
		FVector const NewWorldPos = ConstrainLocationToPlane(NewLocalToWorld.TransformPosition(LocalBasePos) + BaseOffset);
		DeltaPosition = ConstrainDirectionToPlane(NewWorldPos - UpdatedComponent->GetComponentLocation());

		// move attached actor
		if (bFastAttachedMove)
		{
			// we're trusting no other obstacle can prevent the move here
			UpdatedComponent->SetWorldLocationAndRotation(NewWorldPos, FinalQuat, false);
		}
		else
		{
			FHitResult MoveOnBaseHit(1.f);
			const FVector OldLocation = UpdatedComponent->GetComponentLocation();
			MoveUpdatedComponent(DeltaPosition, FinalQuat, true, &MoveOnBaseHit);
			if ((UpdatedComponent->GetComponentLocation() - (OldLocation + DeltaPosition)).IsNearlyZero() == false)
			{
				OnUnableToFollowBaseMove(DeltaPosition, OldLocation, MoveOnBaseHit);
			}
		}

		if (MovementBase->IsSimulatingPhysics() && CharacterOwner->GetMesh())
		{
			CharacterOwner->GetMesh()->ApplyDeltaToAllPhysicsTransforms(DeltaPosition, DeltaQuat);
		}
	}
}

bool UGravityMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		const FVector JumpDir = GetUpVector();

		// If movement isn't constrained or the angle between plane normal and jump direction is between 60 and 120 degrees...
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal | JumpDir) < 0.5f)
		{
			UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
			if (IsValid(MovementBase))
			{
				BaseAngularVelocity = MovementBase->GetPhysicsAngularVelocityInDegrees();
			}

			// Set to zero the vertical component of velocity.
			Velocity = FVector::VectorPlaneProject(Velocity, JumpDir);

			// Perform jump.
			Velocity += JumpDir * JumpZVelocity;
			SetMovementMode(MOVE_Falling);

			return true;
		}
	}

	return false;
}

FVector UGravityMovementComponent::GetImpartedMovementBaseVelocity() const
{
	FVector Result = FVector::ZeroVector;
	if (CharacterOwner)
	{
		UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
		if (MovementBaseUtility::IsDynamicBase(MovementBase))
		{
			FVector BaseVelocity = MovementBaseUtility::GetMovementBaseVelocity(MovementBase, CharacterOwner->GetBasedMovement().BoneName);

			if (bImpartBaseAngularVelocity)
			{
				const FVector CharacterBasePosition = (UpdatedComponent->GetComponentLocation() - GetUpVector() * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
				const FVector BaseTangentialVel = MovementBaseUtility::GetMovementBaseTangentialVelocity(MovementBase, CharacterOwner->GetBasedMovement().BoneName, CharacterBasePosition);
				BaseVelocity += BaseTangentialVel;
			}

			if (bImpartBaseVelocityX)
			{
				Result.X = BaseVelocity.X;
			}
			if (bImpartBaseVelocityY)
			{
				Result.Y = BaseVelocity.Y;
			}
			if (bImpartBaseVelocityZ)
			{
				Result.Z = BaseVelocity.Z;
			}
		}
	}

	return Result;
}

void UGravityMovementComponent::JumpOff(AActor* MovementBaseActor)
{
	if (!bPerformingJumpOff)
	{
		bPerformingJumpOff = true;
		if (CharacterOwner)
		{
			const float MaxSpeed = GetMaxSpeed() * 0.85f;
			Velocity += GetBestDirectionOffActor(MovementBaseActor) * MaxSpeed;

			const FVector JumpDir = GetUpVector();
			FVector Velocity2D = FVector::VectorPlaneProject(Velocity, JumpDir);
			if (Velocity2D.Size() > MaxSpeed)
			{
				Velocity2D = FVector::VectorPlaneProject(Velocity.GetSafeNormal() * MaxSpeed, JumpDir);
			}

			Velocity = Velocity2D + JumpDir * (JumpZVelocity * JumpOffJumpZFactor);
			SetMovementMode(MOVE_Falling);
		}

		bPerformingJumpOff = false;
	}
}

bool UGravityMovementComponent::FloorSweepTest(
	struct FHitResult& OutHit,
	const FVector& Start,
	const FVector& End,
	ECollisionChannel TraceChannel,
	const struct FCollisionShape& CollisionShape,
	const struct FCollisionQueryParams& Params,
	const struct FCollisionResponseParams& ResponseParam
	) const
{
	const bool bBlockingHit = GetWorld()->SweepSingleByChannel(OutHit, Start, End, GetCapsuleRotation(), TraceChannel, CollisionShape, Params, ResponseParam);

	if (bBlockingHit && bUseFlatBaseForFloorChecks)
	{
		FHitResult Hit(1.f);
		const FVector SweepAxis = (End - Start).GetSafeNormal();
		const float SweepSize = (End - Start).Size();

		// Search for floor gaps.
		if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, Start + SweepAxis * (SweepSize + CollisionShape.GetCapsuleHalfHeight()), TraceChannel, Params, ResponseParam))
		{
			// Get the intersection point of the sweep axis and the impact plane.
			const FVector IntersectionPoint = FMath::LinePlaneIntersection(Start, End, OutHit.ImpactPoint, OutHit.ImpactNormal);

			// Calculate the new 'time' of impact along the sweep axis direction.
			const float NewTime = (IntersectionPoint + SweepAxis * (CollisionShape.GetCapsuleHalfHeight() * -1.f) - Start).Size() / SweepSize;

			// Always keep the lowest 'time'.
			OutHit.Time = FMath::Min(OutHit.Time, NewTime);
		}
	}

	return bBlockingHit;
}

bool UGravityMovementComponent::IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const
{
	if (!Hit.bBlockingHit)
	{
		return false;
	}

	const FVector CapsuleDown = GetUpVector() * -1.f;

	// Skip some checks if penetrating. Penetration will be handled by the FindFloor call (using a smaller capsule).
	if (!Hit.bStartPenetrating)
	{
		// Reject unwalkable floor normals.
		if (!IsWalkable(Hit))
		{
			return false;
		}

		float PawnRadius, PawnHalfHeight;
		CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

		// Get the axis of the capsule bounded by the following two end points.
		const FVector BottomPoint = Hit.Location + CapsuleDown * FMath::Max(0.f, PawnHalfHeight - PawnRadius);
		const FVector TopPoint = Hit.Location - CapsuleDown;
		const FVector Segment = TopPoint - BottomPoint;

		// Project the impact point on the segment.
		const float Alpha = ((Hit.ImpactPoint - BottomPoint) | Segment) / Segment.SizeSquared();

		// Reject hits that are above our lower hemisphere (can happen when sliding "down" a vertical surface).
		if (Alpha >= 0.f)
		{
			return false;
		}

		// Reject hits that are barely on the cusp of the radius of the capsule.
		if (!IsWithinEdgeTolerance(Hit.Location, Hit.ImpactPoint, PawnRadius))
		{
			return false;
		}
	}
	else
	{
		// Penetrating.
		if ((Hit.Normal | CapsuleDown) > -KINDA_SMALL_NUMBER)
		{
			// Normal is nearly horizontal or downward, that's a penetration adjustment next to a vertical or overhanging wall. Don't pop to the floor.
			return false;
		}
	}

	FFindFloorResult FloorResult;
	FindFloor(CapsuleLocation, FloorResult, false, &Hit);

	// Reject invalid surfaces.
	if (!FloorResult.IsWalkableFloor())
	{
		return false;
	}

	return true;
}

bool UGravityMovementComponent::ShouldCheckForValidLandingSpot(float DeltaTime, const FVector& Delta, const FHitResult& Hit) const
{
	const FVector CapsuleUp = GetUpVector();

	// See if we hit an edge of a surface on the lower portion of the capsule.
	// In this case the normal will not equal the impact normal, and a downward sweep may find a walkable surface on top of the edge.
	if ((Hit.Normal | CapsuleUp) > KINDA_SMALL_NUMBER && !Hit.Normal.Equals(Hit.ImpactNormal) &&
		IsWithinEdgeTolerance(UpdatedComponent->GetComponentLocation(), Hit.ImpactPoint, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius()))
	{
		return true;
	}

	return false;
}

bool UGravityMovementComponent::ShouldComputePerchResult(const FHitResult& InHit, bool bCheckRadius /*= true*/) const
{
	if (!InHit.IsValidBlockingHit())
	{
		return false;
	}

	// Don't try to perch if the edge radius is very small.
	if (GetPerchRadiusThreshold() <= SWEEP_EDGE_REJECT_DISTANCE)
	{
		return false;
	}

	if (bCheckRadius)
	{
		const FVector CapsuleDown = GetUpVector() * -1.f;
		const float DistFromCenterSq = (InHit.Location + CapsuleDown * ((InHit.ImpactPoint - InHit.Location) | CapsuleDown) - InHit.ImpactPoint).SizeSquared();
		const float StandOnEdgeRadiusSq = FMath::Square(GetValidPerchRadius());

		if (DistFromCenterSq <= StandOnEdgeRadiusSq)
		{
			// Already within perch radius.
			return false;
		}
	}

	return true;
}

bool UGravityMovementComponent::ComputePerchResult(const float TestRadius, const FHitResult& InHit, const float InMaxFloorDist, FFindFloorResult& OutPerchFloorResult) const
{
	if (InMaxFloorDist <= 0.f)
	{
		return 0.f;
	}

	// Sweep further than actual requested distance, because a reduced capsule radius means we could miss some hits that the normal radius would contact.
	float PawnRadius, PawnHalfHeight;
	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	const FVector CapsuleDown = GetUpVector() * -1.f;
	const float InHitAboveBase = (InHit.Location + CapsuleDown * ((InHit.ImpactPoint - InHit.Location) | CapsuleDown) -
		(InHit.Location + CapsuleDown * PawnHalfHeight)).Size();
	const float PerchLineDist = FMath::Max(0.f, InMaxFloorDist - InHitAboveBase);
	const float PerchSweepDist = FMath::Max(0.f, InMaxFloorDist);

	const float ActualSweepDist = PerchSweepDist + PawnRadius;
	ComputeFloorDist(InHit.Location, PerchLineDist, ActualSweepDist, OutPerchFloorResult, TestRadius);

	if (!OutPerchFloorResult.IsWalkableFloor())
	{
		return false;
	}
	else if (InHitAboveBase + OutPerchFloorResult.FloorDist > InMaxFloorDist)
	{
		// Hit something past max distance.
		OutPerchFloorResult.bWalkableFloor = false;
		return false;
	}

	return true;
}

bool UGravityMovementComponent::StepUp(const FVector& GravDir, const FVector& Delta, const FHitResult &InHit, FStepDownResult* OutStepDownResult /*= NULL*/)
{
	//SCOPE_CYCLE_COUNTER(STAT_CharStepUp);

	if (!CanStepUp(InHit) || MaxStepHeight <= 0.f)
	{
		return false;
	}

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	float PawnRadius, PawnHalfHeight;
	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	const FVector CapsuleDown = GetUpVector() * -1.f;

	// Get the axis of the capsule bounded by the following two end points.
	const FVector BottomPoint = OldLocation + CapsuleDown * PawnHalfHeight;
	const FVector TopPoint = OldLocation - CapsuleDown * FMath::Max(0.f, PawnHalfHeight - PawnRadius);
	const FVector Segment = TopPoint - BottomPoint;

	// Project the impact point on the segment.
	const float Alpha = ((InHit.ImpactPoint - BottomPoint) | Segment) / Segment.SizeSquared();

	// Don't bother stepping up if top of capsule is hitting something or if the impact is below us.
	if (Alpha > 1.f || Alpha <= 0.f)
	{
		return false;
	}

	float StepTravelUpHeight = MaxStepHeight;
	float StepTravelDownHeight = StepTravelUpHeight;
	const float StepSideZ = (InHit.ImpactNormal | GravDir) * -1.f;
	FVector PawnInitialFloorBase = OldLocation + CapsuleDown * PawnHalfHeight;
	FVector PawnFloorPoint = PawnInitialFloorBase;

	if (IsMovingOnGround() && CurrentFloor.IsWalkableFloor())
	{
		// Since we float a variable amount off the floor, we need to enforce max step height off the actual point of impact with the floor.
		const float FloorDist = FMath::Max(0.f, CurrentFloor.FloorDist);
		PawnInitialFloorBase += CapsuleDown * FloorDist;
		StepTravelUpHeight = FMath::Max(StepTravelUpHeight - FloorDist, 0.f);
		StepTravelDownHeight = (MaxStepHeight + MAX_FLOOR_DIST * 2.f);

		const bool bHitVerticalFace = !IsWithinEdgeTolerance(InHit.Location, InHit.ImpactPoint, PawnRadius);
		if (!CurrentFloor.bLineTrace && !bHitVerticalFace)
		{
			PawnFloorPoint = CurrentFloor.HitResult.ImpactPoint;
		}
		else
		{
			// Base floor point is the base of the capsule moved down by how far we are hovering over the surface we are hitting.
			PawnFloorPoint += CapsuleDown * CurrentFloor.FloorDist;
		}
	}

	// Scope our movement updates, and do not apply them until all intermediate moves are completed.
	FScopedMovementUpdate ScopedStepUpMovement(UpdatedComponent, EScopedUpdate::DeferredUpdates);

	// step up - treat as vertical wall
	FHitResult SweepUpHit(1.f);
	const FQuat PawnRotation = UpdatedComponent->GetComponentQuat();
	MoveUpdatedComponent(-GravDir * StepTravelUpHeight, PawnRotation, true, &SweepUpHit);

	if (SweepUpHit.bStartPenetrating)
	{
		// Undo movement
		ScopedStepUpMovement.RevertMove();
		return false;
	}

	// step forward.
	FHitResult Hit(1.f);
	MoveUpdatedComponent(Delta, PawnRotation, true, &Hit);

	// Check result of forward movement.
	if (Hit.bBlockingHit)
	{
		if (Hit.bStartPenetrating)
		{
			// Undo movement
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// If we hit something above us and also something ahead of us, we should notify about the upward hit as well.
		// The forward hit will be handled later (in the bSteppedOver case below).
		// In the case of hitting something above but not forward, we are not blocked from moving so we don't need the notification.
		if (SweepUpHit.bBlockingHit && Hit.bBlockingHit)
		{
			HandleImpact(SweepUpHit);
		}

		// pawn ran into a wall.
		HandleImpact(Hit);
		if (IsFalling())
		{
			return true;
		}

		// Adjust and try again.
		const float ForwardSweepFwdHitTime = Hit.Time;
		const float ForwardSlideAmount = SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);

		if (IsFalling())
		{
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// If both the forward SweepFwdHit and the deflection got us nowhere, there is no point in this step up.
		if (ForwardSweepFwdHitTime == 0.f && ForwardSlideAmount == 0.f)
		{
			ScopedStepUpMovement.RevertMove();
			return false;
		}
	}

	// Step down.
	MoveUpdatedComponent(GravDir * StepTravelDownHeight, UpdatedComponent->GetComponentQuat(), true, &Hit);

	// If step down was initially penetrating abort the step up.
	if (Hit.bStartPenetrating)
	{
		ScopedStepUpMovement.RevertMove();
		return false;
	}

	FStepDownResult StepDownResult;
	if (Hit.IsValidBlockingHit())
	{
		// See if this step sequence would have allowed us to travel higher than our max step height allows.
		const float DeltaZ = (PawnFloorPoint - Hit.ImpactPoint) | CapsuleDown;
		if (DeltaZ > MaxStepHeight)
		{
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// Reject unwalkable surface normals here.
		if (!IsWalkable(Hit))
		{
			// Reject if normal opposes movement direction.
			const bool bNormalTowardsMe = (Delta | Hit.ImpactNormal) < 0.f;
			if (bNormalTowardsMe)
			{
				ScopedStepUpMovement.RevertMove();
				return false;
			}

			// Also reject if we would end up being higher than our starting location by stepping down.
			// It's fine to step down onto an unwalkable normal below us, we will just slide off. Rejecting those moves would prevent us from being able to walk off the edge.
			if (((OldLocation - Hit.Location) | CapsuleDown) > 0.f)
			{
				ScopedStepUpMovement.RevertMove();
				return false;
			}
		}

		// Reject moves where the downward sweep hit something very close to the edge of the capsule. This maintains consistency with FindFloor as well.
		if (!IsWithinEdgeTolerance(Hit.Location, Hit.ImpactPoint, PawnRadius))
		{
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// Don't step up onto invalid surfaces if traveling higher.
		if (DeltaZ > 0.f && !CanStepUp(Hit))
		{
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// See if we can validate the floor as a result of this step down. In almost all cases this should succeed, and we can avoid computing the floor outside this method.
		if (OutStepDownResult != NULL)
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), StepDownResult.FloorResult, false, &Hit);

			// Reject unwalkable normals if we end up higher than our initial height.
			// It's fine to walk down onto an unwalkable surface, don't reject those moves.
			if (((OldLocation - Hit.Location) | CapsuleDown) > 0.f)
			{
				// We should reject the floor result if we are trying to step up an actual step where we are not able to perch (this is rare).
				// In those cases we should instead abort the step up and try to slide along the stair.
				if (!StepDownResult.FloorResult.bBlockingHit && StepSideZ < MAX_STEP_SIDE_Z)
				{
					ScopedStepUpMovement.RevertMove();
					return false;
				}
			}

			StepDownResult.bComputedFloor = true;
		}
	}

	// Copy step down result.
	if (OutStepDownResult != NULL)
	{
		*OutStepDownResult = StepDownResult;
	}

	// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments.
	bJustTeleported |= !bMaintainHorizontalGroundVelocity;

	return true;
}

void UGravityMovementComponent::SetDefaultMovementMode()
{
	// check for water volume
	if (CanEverSwim() && IsInWater())
	{
		SetMovementMode(DefaultWaterMovementMode);
	}
	else if (!CharacterOwner || MovementMode != DefaultLandMovementMode)
	{
		SetMovementMode(DefaultLandMovementMode);

		// Avoid 1-frame delay if trying to walk but walking fails at this location.
		if (MovementMode == MOVE_Walking && GetMovementBase() == NULL)
		{
			SetMovementMode(MOVE_Falling);
		}
	}
}

void UGravityMovementComponent::AdjustFloorHeight()
{
	//SCOPE_CYCLE_COUNTER(STAT_CharAdjustFloorHeight);

	// If we have a floor check that hasn't hit anything, don't adjust height.
	if (!CurrentFloor.IsWalkableFloor())
	{
		return;
	}

	float OldFloorDist = CurrentFloor.FloorDist;
	if (CurrentFloor.bLineTrace)
	{
		if (OldFloorDist < MIN_FLOOR_DIST && CurrentFloor.LineDist >= MIN_FLOOR_DIST)
		{
			// This would cause us to scale unwalkable walls
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("Adjust floor height aborting due to line trace with small floor distance (line: %.2f, sweep: %.2f)"), CurrentFloor.LineDist, CurrentFloor.FloorDist);
			return;
		}
		else
		{
			// Falling back to a line trace means the sweep was unwalkable (or in penetration). Use the line distance for the vertical adjustment.
			OldFloorDist = CurrentFloor.LineDist;
		}
	}

	// Move up or down to maintain floor height.
	if (OldFloorDist < MIN_FLOOR_DIST || OldFloorDist > MAX_FLOOR_DIST)
	{
		FHitResult AdjustHit(1.f);
		const FVector CapsuleUp = GetUpVector();
		const FVector InitialLocation = UpdatedComponent->GetComponentLocation();
		const float AvgFloorDist = (MIN_FLOOR_DIST + MAX_FLOOR_DIST) * 0.5f;
		const float MoveDist = AvgFloorDist - OldFloorDist;
		SafeMoveUpdatedComponent(CapsuleUp * MoveDist, CharacterOwner->GetActorRotation(), true, AdjustHit);
		//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("Adjust floor height %.3f (Hit = %d)"), MoveDist, AdjustHit.bBlockingHit);

		if (!AdjustHit.IsValidBlockingHit())
		{
			CurrentFloor.FloorDist += MoveDist;
		}
		else if (MoveDist > 0.f)
		{
			CurrentFloor.FloorDist += (InitialLocation - UpdatedComponent->GetComponentLocation()) | CapsuleUp;
		}
		else
		{
			checkSlow(MoveDist < 0.f);
			CurrentFloor.FloorDist = (AdjustHit.Location - UpdatedComponent->GetComponentLocation()) | CapsuleUp;
			if (IsWalkable(AdjustHit))
			{
				CurrentFloor.SetFromSweep(AdjustHit, CurrentFloor.FloorDist, true);
			}
		}

		// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments.
		// Also avoid it if we moved out of penetration.
		bJustTeleported |= !bMaintainHorizontalGroundVelocity || OldFloorDist < 0.f;
	
		// If something caused us to adjust our height (especially a depentration) we should ensure another check next frame or we will keep a stale result.
		if (CharacterOwner && CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
		{
			bForceNextFloorCheck = true;
		}
	}
}

bool UGravityMovementComponent::CheckLedgeDirection(const FVector& OldLocation, const FVector& SideStep, const FVector& GravDir) const
{
	const FVector SideDest = OldLocation + SideStep;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CheckLedgeDirection), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);
	const FCollisionShape CapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_None);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	FHitResult Result(1.f);
	GetWorld()->SweepSingleByChannel(Result, OldLocation, SideDest, GetCapsuleRotation(), CollisionChannel, CapsuleShape, CapsuleParams, ResponseParam);

	if (!Result.bBlockingHit || IsWalkable(Result))
	{
		if (!Result.bBlockingHit)
		{
			GetWorld()->SweepSingleByChannel(Result, SideDest, SideDest + GravDir * (MaxStepHeight + LedgeCheckThreshold), GetCapsuleRotation(), CollisionChannel, CapsuleShape, CapsuleParams, ResponseParam);
		}

		if ((Result.Time < 1.f) && IsWalkable(Result))
		{
			return true;
		}
	}

	return false;
}

FVector UGravityMovementComponent::GetLedgeMove(const FVector& OldLocation, const FVector& Delta, const FVector& GravDir) const
{
	if (!HasValidData() || Delta.IsZero())
	{
		return FVector::ZeroVector;
	}

	FVector SideDir = FVector::VectorPlaneProject(Delta, GravDir);

	// try left.
	SideDir = FQuat(GravDir, PI * 0.5f).RotateVector(SideDir);
	if (CheckLedgeDirection(OldLocation, SideDir, GravDir))
	{
		return SideDir;
	}

	// try right.
	SideDir *= -1.f;
	if (CheckLedgeDirection(OldLocation, SideDir, GravDir))
	{
		return SideDir;
	}

	return FVector::ZeroVector;
}

void UGravityMovementComponent::ApplyAccumulatedForces(float DeltaSeconds)
{
	if (!PendingImpulseToApply.IsZero() || !PendingForceToApply.IsZero())
	{
		const FVector Impulse = PendingImpulseToApply + PendingForceToApply * DeltaSeconds + GetGravity() * DeltaSeconds;

		// Check to see if applied momentum is enough to overcome gravity.
		if ((Impulse | GetUpVector()) > SMALL_NUMBER)
		{
			SetMovementMode(MOVE_Falling);
		}
	}

	Velocity += PendingImpulseToApply + PendingForceToApply * DeltaSeconds;

	// Don't call ClearAccumulatedForces() because it could affect launch velocity
	PendingImpulseToApply = FVector::ZeroVector;
	PendingForceToApply = FVector::ZeroVector;
}

bool UGravityMovementComponent::IsWalkable(const FHitResult& Hit) const
{
	if (!Hit.IsValidBlockingHit())
	{
		// No hit, or starting in penetration.
		return false;
	}

	float TestWalkableZ = GetWalkableFloorZ();

	// See if this component overrides the walkable floor z.
	const UPrimitiveComponent* HitComponent = Hit.Component.Get();
	if (HitComponent)
	{
		const FWalkableSlopeOverride& SlopeOverride = HitComponent->GetWalkableSlopeOverride();
		TestWalkableZ = SlopeOverride.ModifyWalkableFloorZ(TestWalkableZ);
	}

	// Can't walk on this surface if it is too steep.
	if ((Hit.ImpactNormal | GetUpVector()) < TestWalkableZ)
	{
		return false;
	}

	return true;
}

void UGravityMovementComponent::ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult, float SweepRadius, const FHitResult* DownwardSweepResult) const
{
	//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("[Role:%d] ComputeFloorDist: %s at location %s"), (int32)CharacterOwner->GetLocalRole(), *GetNameSafe(CharacterOwner), *CapsuleLocation.ToString());
	OutFloorResult.Clear();

	// No collision, no floor...
	if (!UpdatedComponent->IsCollisionEnabled())
	{
		return;
	}

	float PawnRadius, PawnHalfHeight;
	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	const FVector CapsuleDown = GetUpVector() * -1.f;

	bool bSkipSweep = false;
	if (DownwardSweepResult != NULL && DownwardSweepResult->IsValidBlockingHit())
	{
		const float Dot = CapsuleDown | ((DownwardSweepResult->TraceEnd - DownwardSweepResult->TraceStart).GetSafeNormal());

		// Only if the supplied sweep was vertical and downward.
		if (Dot >= THRESH_NORMALS_ARE_PARALLEL)
		{
			// Reject hits that are barely on the cusp of the radius of the capsule.
			if (IsWithinEdgeToleranceNew(DownwardSweepResult->Location, CapsuleDown, DownwardSweepResult->ImpactPoint, PawnRadius))
			{
				// Don't try a redundant sweep, regardless of whether this sweep is usable.
				bSkipSweep = true;

				const bool bIsWalkable = IsWalkable(*DownwardSweepResult);
				const float FloorDist = (CapsuleLocation - DownwardSweepResult->Location).Size();
				OutFloorResult.SetFromSweep(*DownwardSweepResult, FloorDist, bIsWalkable);

				if (bIsWalkable)
				{
					// Use the supplied downward sweep as the floor hit result.
					return;
				}
			}
		}
	}

	// We require the sweep distance to be >= the line distance, otherwise the HitResult can't be interpreted as the sweep result.
	if (SweepDistance < LineDistance)
	{
		check(SweepDistance >= LineDistance);
		return;
	}

	bool bBlockingHit = false;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ComputeFloorDist), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(QueryParams, ResponseParam);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	// Sweep test.
	if (!bSkipSweep && SweepDistance > 0.f && SweepRadius > 0.f)
	{
		// Use a shorter height to avoid sweeps giving weird results if we start on a surface.
		// This also allows us to adjust out of penetrations.
		const float ShrinkScale = 0.9f;
		const float ShrinkScaleOverlap = 0.1f;
		float ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScale);
		float TraceDist = SweepDistance + ShrinkHeight;
		FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(SweepRadius, PawnHalfHeight - ShrinkHeight);

		FHitResult Hit(1.f);
		bBlockingHit = FloorSweepTest(Hit, CapsuleLocation, CapsuleLocation + CapsuleDown * TraceDist, CollisionChannel, CapsuleShape, QueryParams, ResponseParam);

		if (bBlockingHit)
		{
			// Reject hits adjacent to us, we only care about hits on the bottom portion of our capsule.
			// Check 2D distance to impact point, reject if within a tolerance from radius.
			if (Hit.bStartPenetrating || !IsWithinEdgeToleranceNew(CapsuleLocation, CapsuleDown, Hit.ImpactPoint, CapsuleShape.Capsule.Radius))
			{
				// Use a capsule with a slightly smaller radius and shorter height to avoid the adjacent object.
				// Capsule must not be nearly zero or the trace will fall back to a line trace from the start point and have the wrong length.
				CapsuleShape.Capsule.Radius = FMath::Max(0.f, CapsuleShape.Capsule.Radius - SWEEP_EDGE_REJECT_DISTANCE - KINDA_SMALL_NUMBER);
				if (!CapsuleShape.IsNearlyZero())
				{
					ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScaleOverlap);
					TraceDist = SweepDistance + ShrinkHeight;
					CapsuleShape.Capsule.HalfHeight = FMath::Max(PawnHalfHeight - ShrinkHeight, CapsuleShape.Capsule.Radius);
					Hit.Reset(1.f, false);

					bBlockingHit = FloorSweepTest(Hit, CapsuleLocation, CapsuleLocation + CapsuleDown * TraceDist, CollisionChannel, CapsuleShape, QueryParams, ResponseParam);
				}
			}

			// Reduce hit distance by ShrinkHeight because we shrank the capsule for the trace.
			// We allow negative distances here, because this allows us to pull out of penetrations.
			const float MaxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, PawnRadius);
			const float SweepResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

			OutFloorResult.SetFromSweep(Hit, SweepResult, false);
			if (Hit.IsValidBlockingHit() && IsWalkable(Hit))
			{
				if (SweepResult <= SweepDistance)
				{
					// Hit within test distance.
					OutFloorResult.bWalkableFloor = true;
					return;
				}
			}
		}
	}

	// Since we require a longer sweep than line trace, we don't want to run the line trace if the sweep missed everything.
	// We do however want to try a line trace if the sweep was stuck in penetration.
	if (!OutFloorResult.bBlockingHit && !OutFloorResult.HitResult.bStartPenetrating)
	{
		OutFloorResult.FloorDist = SweepDistance;
		return;
	}

	// Line trace.
	if (LineDistance > 0.f)
	{
		const float ShrinkHeight = PawnHalfHeight;
		const FVector LineTraceStart = CapsuleLocation;
		const float TraceDist = LineDistance + ShrinkHeight;
		QueryParams.TraceTag = SCENE_QUERY_STAT_NAME_ONLY(FloorLineTrace);

		FHitResult Hit(1.f);
		bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, LineTraceStart, LineTraceStart + CapsuleDown * TraceDist, CollisionChannel, QueryParams, ResponseParam);

		if (bBlockingHit && Hit.Time > 0.f)
		{
			if (Hit.Time > 0.f)
			{
				// Reduce hit distance by ShrinkHeight because we started the trace higher than the base.
				// We allow negative distances here, because this allows us to pull out of penetrations.
				const float MaxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, PawnRadius);
				const float LineResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

				OutFloorResult.bBlockingHit = true;
				if (LineResult <= LineDistance && IsWalkable(Hit))
				{
					OutFloorResult.SetFromLineTrace(Hit, OutFloorResult.FloorDist, LineResult, true);
					return;
				}
			}
		}
	}

	// No hits were acceptable.
	OutFloorResult.bWalkableFloor = false;
}

bool UGravityMovementComponent::IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const
{
	const FVector CapsuleDown = GetUpVector() * -1.f;
	const float DistFromCenterSq = (CapsuleLocation + CapsuleDown * ((TestImpactPoint - CapsuleLocation) | CapsuleDown) - TestImpactPoint).SizeSquared();
	const float ReducedRadiusSq = FMath::Square(FMath::Max(KINDA_SMALL_NUMBER, CapsuleRadius - SWEEP_EDGE_REJECT_DISTANCE));
	return DistFromCenterSq < ReducedRadiusSq;
}

FVector UGravityMovementComponent::GetGravity() const
{
	return GravityDirection * GravityAccel * GravityScale;
}

void UGravityMovementComponent::UpdateComponentRotation()
{
	if (!UpdatedComponent || GetGravity().IsZero())
	{
		return;
	}

	// 如果新旧两个"向上"的向量夹角几乎为0度，则不调整旋转
	const FVector DesiredCapsuleUp = GravityDirection * -1.f;
	if ((DesiredCapsuleUp | GetUpVector()) >= THRESH_NORMALS_ARE_PARALLEL)
	{
		return;
	}

	// Take desired Z rotation axis of capsule, try to keep current X rotation axis of capsule.
	const FMatrix RotationMatrix = FRotationMatrix::MakeFromZX(DesiredCapsuleUp, GetForwardVector());
	const FRotator TargetRotator = FMath::RInterpTo(
		GetCapsuleRotation().Rotator(),
		RotationMatrix.Rotator(),
		GetWorld()->GetDeltaSeconds(),
		GravityAccel*0.001);

	// Intentionally not using MoveUpdatedComponent to bypass constraints.
	MoveUpdatedComponent(FVector::ZeroVector, TargetRotator, true);
}

FORCEINLINE FQuat UGravityMovementComponent::GetCapsuleRotation() const
{
	return UpdatedComponent->GetComponentQuat();
}

FORCEINLINE FVector UGravityMovementComponent::GetForwardVector() const
{
	return UpdatedComponent->GetForwardVector();
}

FORCEINLINE FVector UGravityMovementComponent::GetRightVector() const
{
	return UpdatedComponent->GetRightVector();
}

FORCEINLINE FVector UGravityMovementComponent::GetUpVector() const
{
	return UpdatedComponent->GetUpVector();
}

// Version that does not use inverse sqrt estimate, for higher precision.
FORCEINLINE FVector UGravityMovementComponent::GetSafeNormalPrecise(const FVector& V)
{
	const float VSq = V.SizeSquared();
	if (VSq < SMALL_NUMBER)
	{
		return FVector::ZeroVector;
	}
	else
	{
		return V * (1.f / FMath::Sqrt(VSq));
	}
}

bool UGravityMovementComponent::IsWithinEdgeToleranceNew(const FVector& CapsuleLocation, const FVector& CapsuleDown, const FVector& TestImpactPoint, const float CapsuleRadius) const
{
	const float DistFromCenterSq = (CapsuleLocation + CapsuleDown * ((TestImpactPoint - CapsuleLocation) | CapsuleDown) - TestImpactPoint).SizeSquared();
	const float ReducedRadiusSq = FMath::Square(FMath::Max(KINDA_SMALL_NUMBER, CapsuleRadius - SWEEP_EDGE_REJECT_DISTANCE));

	return DistFromCenterSq < ReducedRadiusSq;
}
