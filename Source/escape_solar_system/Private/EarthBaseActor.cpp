// Copyright 2020 H₂S. All Rights Reserved.

#include "EarthBaseActor.h"
#include "MainLevelScript.h"
#include "BackpackComponent.h"
#include "MainCharacter.h"
#include "NPC.h"
#include "Spaceship.h"

AEarthBase::AEarthBase()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CharScope = CreateDefaultSubobject<UBoxComponent>(TEXT("CharScope"));
	ShipScope = CreateDefaultSubobject<USphereComponent>(TEXT("ShipScope"));
	Backpack = CreateDefaultSubobject<UBackpackComponent>(TEXT("Backpack"));

	CharScope->SetupAttachment(GetRootComponent());
	ShipScope->SetupAttachment(GetRootComponent());

	ShipScope->OnComponentBeginOverlap.AddDynamic(this, &AEarthBase::OnShipScopeBeginOverlap);
	ShipScope->OnComponentEndOverlap.AddDynamic(this, &AEarthBase::OnShipScopeEndOverlap);

	// FClassFinder只能在构造函数中调用
	BP_MainCharClass = ConstructorHelpers::FClassFinder<AMainCharacter>(TEXT("Blueprint'/Game/MainBP/Blueprints/BP_MainCharacter.BP_MainCharacter_C'")).Class;
	BP_SpaceshipClass = ConstructorHelpers::FClassFinder<ASpaceship>(TEXT("Blueprint'/Game/MainBP/Blueprints/BP_Spaceship.BP_Spaceship_C'")).Class;
	BP_GuideNPCClass = ConstructorHelpers::FClassFinder<ANPC>(TEXT("Blueprint'/Game/MainBP/Blueprints/BP_GuideNPC.BP_GuideNPC_C'")).Class;
}

void AEarthBase::CreateMainChar()
{
	check(!AMainLevelScript::GetMainChar());
	const auto SelfTransform = GetActorTransform();
	GetWorld()->SpawnActor<AMainCharacter>(BP_MainCharClass,
		SelfTransform.TransformPosition(CharBirthplace.GetLocation()),
		SelfTransform.TransformRotation(CharBirthplace.GetRotation()).Rotator());
	if (GuideNPC == nullptr)
	{
		GuideNPC = GetWorld()->SpawnActor<ANPC>(BP_GuideNPCClass, GetActorTransform());
		AMainLevelScript::GetMainChar()->SetTalkableNPC(GuideNPC);
		AMainLevelScript::Instance()->TalkOpenedDelegate.Broadcast();
	}
}

void AEarthBase::CreateSpaceship()
{
	check(!AMainLevelScript::GetSpaceship());
	const auto SelfTransform = GetActorTransform();
	GetWorld()->SpawnActor<ASpaceship>(BP_SpaceshipClass,
		SelfTransform.TransformPosition(ShipBirthplace.GetLocation()),
		SelfTransform.TransformRotation(ShipBirthplace.GetRotation()).Rotator());
}

AMainCharacter* AEarthBase::FindMainChar() const
{
	TArray<AActor*> NearbyActors;
	CharScope->GetOverlappingActors(NearbyActors, AMainCharacter::StaticClass());
	if (NearbyActors.Num() > 0)
	{
		return Cast<AMainCharacter>(NearbyActors[0]);
	}
	return nullptr;
}

ASpaceship* AEarthBase::FindSpaceship() const
{
	TArray<AActor*> NearbyActors;
	ShipScope->GetOverlappingActors(NearbyActors, ASpaceship::StaticClass());
	if (NearbyActors.Num() > 0)
	{
		return Cast<ASpaceship>(NearbyActors[0]);
	}
	return nullptr;
}

void AEarthBase::BeginPlay()
{
	Super::BeginPlay();
	AMainLevelScript::SetEarthBase(this);
}

void AEarthBase::OnShipScopeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ASpaceship>())
	{
		OnShipEnter();
	}
}

void AEarthBase::OnShipScopeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<ASpaceship>())
	{
		OnShipLeave();
	}
}
