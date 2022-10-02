// Copyright 2020 H₂S. All Rights Reserved.

#include "CelestialIndicator.h"
#include "CelestialBody.h"
#include "MainLevelScript.h"
#include <Kismet/GameplayStatics.h>

void UCelestialIndicatorHelper::GetLockInfo(const ACelestialBody* Body, float& Dist, float& Speed)
{
	const APawn* Player = UGameplayStatics::GetPlayerPawn(GWorld, 0);
	if (Body && Player)
	{
		const double DistValue = FVector::Dist(Body->GetActorLocation(), Player->GetActorLocation());
		Dist = (DistValue - Body->GetSelfRadius()) / 100.f;
		Speed = (DistValue - LastDist) / GWorld->GetDeltaSeconds() / 100.f;

		LastDist = DistValue;
	}
}

void UCelestialIndicatorHelper::GetWidgetPosition(const ACelestialBody* Body, FVector2D& Position, int32& Angle)
{
	Angle = -1;
	Position = FVector2D::ZeroVector;
	const auto CameraMgr = UGameplayStatics::GetPlayerCameraManager(GWorld, 0);
	if (Body && CameraMgr)
	{
		const FVector PlayerLoc = CameraMgr->GetCameraLocation();
		const FVector BodyLoc = Body->GetActorLocation();

		const FVector SelfDir = (BodyLoc - PlayerLoc).GetSafeNormal();
		const FVector CameraDir = CameraMgr->GetCameraRotation().Vector();
		// 大于90°，就是已经在背后了，需要镜像到前面
		const bool bIsBehind = FMath::Acos(SelfDir | CameraDir) > PI * 0.5;
		UGameplayStatics::ProjectWorldToScreen(
			UGameplayStatics::GetPlayerController(GWorld, 0),
			bIsBehind ? BodyLoc.MirrorByPlane(FPlane(PlayerLoc, CameraDir)) : BodyLoc,
			Position,
			false
		);
		//UE_LOG(LogTemp, Error, TEXT("%s"), *(Position.ToString()));
		
		bool bOutside = false;
		FVector2D SceenSize = FVector2D::ZeroVector;
		GEngine->GameViewport->GetViewportSize(SceenSize);
		// 转换到以屏幕中心为原点
		Position -= SceenSize * 0.5;
		const float MinX = -SceenSize.X * 0.5 + 64.f;
		const float MaxX = SceenSize.X * 0.5 - 64.f;
		const float MinY = -SceenSize.Y * 0.5 + 32.f;
		const float MaxY = SceenSize.Y * 0.5 - 32.f;
		float ScaleX = 1;
		float ScaleY = 1;
		if (Position.X < MinX)
		{
			bOutside = true;
			ScaleX = MinX / Position.X;
		}
		else if (Position.X > MaxX)
		{
			bOutside = true;
			ScaleX = MaxX / Position.X;
		}
		if (Position.Y < MinY)
		{
			bOutside = true;
			ScaleY = MinY / Position.Y;
		}
		else if (Position.Y > MaxY)
		{
			bOutside = true;
			ScaleY = MaxY / Position.Y;
		}

		if (bIsBehind && !bOutside)
		{
			bOutside = true;
			ScaleX = (Position.X < 0 ? MinX : MaxX) / Position.X;
			ScaleY = (Position.Y < 0 ? MinY : MaxY) / Position.Y;
		}
		if (bOutside)
		{
			Angle = FMath::Atan2(Position.Y, Position.X) * 57.29578f + 90.f;
		}

		// 再转换回以左上角为原点的屏幕坐标
		Position = Position * FMath::Min(ScaleX, ScaleY) + SceenSize * 0.5;
	}
}
