// Copyright 2020 H₂S. All Rights Reserved.

#include "NPC.h"
#include "MainPlayerState.h"
#include "MainCharacter.h"
#include "MainLevelScript.h"
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
	MeshComponent->GetBodyInstance()->bSimulatePhysics = true;
	SetRootComponent(MeshComponent);

	SpeakZone = CreateOptionalDefaultSubobject<USphereComponent>(TEXT("SpeakZone"));
	SpeakZone->SetupAttachment(GetRootComponent());
	SpeakZone->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnSpeakZoneBeginOverlap);
	SpeakZone->OnComponentEndOverlap.AddDynamic(this, &ANPC::OnSpeakZoneEndOverlap);

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = MeshComponent;
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
	bSimulatePhysics = MeshComponent->GetBodyInstance()->bSimulatePhysics;
	MeshComponent->GetBodyInstance()->bSimulatePhysics = false;
	Super::BeginPlay(); // AActor::BeginPlay()会判断附着的父对象是否模拟物理，如果不一致，就解除附着
	// 所以模拟物理要在BeginPlay之后开启，而且也不能用UPrimitiveComponent::SetSimulatePhysics开启，因为也会像BeginPlay那样做
	MeshComponent->GetBodyInstance()->bSimulatePhysics = bSimulatePhysics;
	MeshComponent->GetBodyInstance()->UpdateInstanceSimulatePhysics();

	Density = CalcDensity(MeshComponent->BodyInstance.GetMassOverride(), MeshComponent->Bounds.SphereRadius);
}

void ANPC::GravityActed_Implementation(FVector Direction, float Accel)
{
	if (bSimulatePhysics)
	{
		MeshComponent->AddForce(Direction * Accel, NAME_None, true);
	}
}

void ANPC::GravityActedGlobally_Implementation(FVector Direction, float Accel)
{
	if (bSimulatePhysics)
	{
		MeshComponent->AddForce(Direction * Accel, NAME_None, true);
	}
}

void ANPC::BuoyancyActed_Implementation(FVector Force)
{
	if (bSimulatePhysics)
	{
		MeshComponent->AddForce(Force, NAME_None, true);
	}
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

void ANPC::OnSpeakZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (SpeakData && Cast<AMainCharacter>(OtherActor))
	{
		SpeakData->GetAllRows<FSpeakData>(FString(), SpeakList);
		SpeakNext();
	}
}

void ANPC::OnSpeakZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SpeakList.Empty();
	GetWorld()->GetTimerManager().ClearTimer(SpeakTimer);
}

void ANPC::SpeakNext()
{
	if (SpeakList.Num() > 0)
	{
		const auto Data = SpeakList[0];
		SpeakList.RemoveAt(0);
		AMainLevelScript::Instance()->NPCSpeakDelegate.Broadcast(Name, Data->Text, Data->Time);
		GetWorld()->GetTimerManager().SetTimer(SpeakTimer, this, &ANPC::SpeakNext, Data->Time + 2.f, false);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(SpeakTimer);
	}
}

FFormatNamedArguments ANPC::GetVariableArguments() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("DeathCount"), FText::AsNumber(AMainPlayerState::Instance()->GetDeathCount() + 789));
	return Args;
}
