// Copyright 2020 H₂S. All Rights Reserved.

#include "BodyComponent.h"
#include <Kismet/KismetSystemLibrary.h>

UBodyComponent::UBodyComponent()
{
	
}

void UBodyComponent::SetupCollisionComponent(UPrimitiveComponent* PrimitiveComponent)
{
	CollisionComponent = PrimitiveComponent;
}

void UBodyComponent::ChangeHP(int32 Delta)
{
	CurrentHP += Delta;
	if (CurrentHP > MaximumHP)
	{
		CurrentHP = MaximumHP;
	}
	else if (CurrentHP <= 0)
	{
		// TODO 死亡
		UKismetSystemLibrary::PrintText(GetWorld(), INVTEXT("你死了！"));
	}
}

void UBodyComponent::GetHP(int32& CurHP, int32& MaxHP)
{
	MaxHP = MaximumHP;
	CurHP = FMath::Max(0, FMath::Min(CurrentHP, MaximumHP));
}

void UBodyComponent::BeginPlay()
{
	Super::BeginPlay();
	check(CollisionComponent);
	CollisionComponent->OnComponentHit.AddUniqueDynamic(this, &UBodyComponent::OnComponentHitted);
	GetWorld()->GetTimerManager().SetTimer(EnvTimer, this, &UBodyComponent::CheckEnvironment, 1.f, true, 0.f);
}

void UBodyComponent::CheckEnvironment()
{
	// TODO 外界温度影响区域
	CurrentTemp = 0;

	// TODO 外界压力影响区域
	CurrentPress = 0;

	if (CurrentTemp > ShieldHeat)
	{
		ChangeHP(-(CurrentTemp - ShieldHeat));
		UKismetSystemLibrary::PrintText(GetWorld(), FText::Format(INVTEXT("机体过热，扣除{0}点血"), CurrentTemp - ShieldHeat));
	}
	else if (CurrentTemp < ShieldCold)
	{
		ChangeHP(-(ShieldCold - CurrentTemp));
		UKismetSystemLibrary::PrintText(GetWorld(), FText::Format(INVTEXT("机体过冷，扣除{0}点血"), ShieldCold - CurrentTemp));
	}

	if (CurrentTemp > ShieldPress)
	{
		ChangeHP(-(CurrentTemp - ShieldPress));
		UKismetSystemLibrary::PrintText(GetWorld(), FText::Format(INVTEXT("压力过大，扣除{0}点血"), CurrentTemp - ShieldPress));
	}
}

void UBodyComponent::OnComponentHitted(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const float HitSpeed = (HitComponent->GetComponentVelocity() - OtherActor->GetVelocity()).Size();
	if (HitSpeed > 1000)
	{
		ChangeHP(-HitSpeed / 100);
		UKismetSystemLibrary::PrintText(GetWorld(), FText::Format(INVTEXT("受到打击，扣除{0}点血"), HitSpeed / 100));
	}
}

