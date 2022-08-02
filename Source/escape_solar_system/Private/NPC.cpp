// Copyright 2020 H₂S. All Rights Reserved.


#include "NPC.h"

ANPC::ANPC()
{
	//PrimaryActorTick.bCanEverTick = true;
	GetMeshComponent()->SetCollisionProfileName("BlockAllDynamic");
}

FText ANPC::GetText(FName Index) const
{
	if (Index.IsNone()) {
		return FText();
	}
	return TalkData->FindRow<FTalkData>(Index, FString())->Text;
}

void ANPC::GetOptions(FName Index, TMap<FName, FText>& OutOptions) const
{
	OutOptions = TalkData->FindRow<FTalkData>(Index, FString())->Options;
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}
