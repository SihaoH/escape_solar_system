// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GravityMovementComponent.generated.h"

// Custom gravity movement component which moves provides directional gravity movement
UCLASS()
class ESCAPE_SOLAR_SYSTEM_API UGravityMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	UGravityMovementComponent();

public:
	// Begin UCharacterMovementComponent overrides
	virtual FVector CalcAnimRootMotionVelocity(const FVector& RootMotionDeltaMove, float DeltaSeconds, const FVector& CurrentVelocity) const override;
	virtual void StartFalling(int32 Iterations, float remainingTime, float timeTick, const FVector& Delta, const FVector& subLoc) override;
	virtual void PhysFalling(float deltaTime, int32 Iterations) override;
	virtual FVector GetFallingLateralAcceleration(float DeltaTime) override;
	virtual void UpdateBasedMovement(float DeltaSeconds) override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual FVector GetImpartedMovementBaseVelocity() const override;
	virtual void JumpOff(AActor* MovementBaseActor) override;
	virtual bool FloorSweepTest(struct FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, const struct FCollisionShape& CollisionShape, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParam) const override;
	virtual bool IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const override;
	virtual bool ShouldCheckForValidLandingSpot(float DeltaTime, const FVector& Delta, const FHitResult& Hit) const override;
	virtual bool ShouldComputePerchResult(const FHitResult& InHit, bool bCheckRadius = true) const override;
	virtual bool ComputePerchResult(const float TestRadius, const FHitResult& InHit, const float InMaxFloorDist, FFindFloorResult& OutPerchFloorResult) const override;
	virtual bool StepUp(const FVector& GravDir, const FVector& Delta, const FHitResult &Hit, struct UCharacterMovementComponent::FStepDownResult* OutStepDownResult = NULL) override;
	virtual void SetDefaultMovementMode() override;
	virtual void AdjustFloorHeight() override;
	virtual bool CheckLedgeDirection(const FVector& OldLocation, const FVector& SideStep, const FVector& GravDir) const override;
	virtual FVector GetLedgeMove(const FVector& OldLocation, const FVector& Delta, const FVector& GravDir) const override;
	virtual void ApplyAccumulatedForces(float DeltaSeconds) override;
	virtual bool IsWalkable(const FHitResult& Hit) const override;
	virtual void ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult, float SweepRadius, const FHitResult* DownwardSweepResult = NULL) const override;
	virtual bool IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const override;
	// End UCharacterMovementComponent overrides

	FORCEINLINE FQuat GetCapsuleRotation() const;
	FORCEINLINE FVector GetForwardVector() const;
	FORCEINLINE FVector GetRightVector() const;
	FORCEINLINE FVector GetUpVector() const;

protected:
	// Begin UCharacterMovementComponent overrides
	virtual void PhysFlying(float deltaTime, int32 Iterations) override;
	virtual float BoostAirControl(float DeltaTime, float TickAirControl, const FVector& FallAcceleration) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PerformMovement(float DeltaTime) override;
	virtual void HandleImpact(const FHitResult& Impact, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;

	// Tells if we are moving vertical / horizontal -> neccesary for all ground checks and fly
	virtual void MaintainHorizontalGroundVelocity() override;
	virtual float SlideAlongSurface(const FVector& Delta, float Time, const FVector& InNormal, FHitResult& Hit, bool bHandleImpact) override;
	virtual void SetPostLandedPhysics(const FHitResult& Hit) override;
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;
	virtual FVector ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const override;
	virtual void MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult = NULL) override;
	virtual void SimulateMovement(float DeltaTime) override;

	// Setting actual acceleration to be relative to move
	virtual FVector ConstrainInputAcceleration(const FVector& InputAcceleration) const override;
	virtual FVector ScaleInputAcceleration(const FVector& InputAcceleration) const override;
	// End UCharacterMovementComponent overrides

	/** 自定义方向，值为单位向量 */
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite)
	FVector GravityDirection = FVector::ZeroVector;

	/** 自定义重力（加速度），单位cm/s² */
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite)
	float GravityAccel = 0.f;

	/** 基底（角色所站立的星球）的自转角速度，在起跳时获取，为了让角色起跳后能保持跟星球同样的角速度 */
	FVector BaseAngularVelocity = FVector::ZeroVector;

	// When moving debug lines are shown - velocity, acceleration
	UPROPERTY(Category = "Debug", EditAnywhere)
	bool bShowDebugLines = false;

private:
	FVector GetGravity() const;
	void UpdateComponentRotation();

	FORCEINLINE FVector GetSafeNormalPrecise(const FVector& V);
	bool IsWithinEdgeToleranceNew(const FVector& CapsuleLocation, const FVector& CapsuleDown, const FVector& TestImpactPoint, const float CapsuleRadius) const;

	friend class AMainCharacter;
};
