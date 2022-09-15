// Copyright 2020 H₂S. All Rights Reserved.

#include "MainSaveGame.h"
#include "MainLevelScript.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "MainPlayerState.h"
#include "MovableMeshActor.h"
#include "CelestialBody.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <Kismet/GameplayStatics.h>
#include <EngineUtils.h>
#include <UObject/UE5MainStreamObjectVersion.h>
#include "EngineComponent.h"

int64 UMainSaveGame::SaveTimestamp = 0;
bool UMainSaveGame::bIsNeedLoad = false;
FString UMainSaveGame::SaveSlotName = TEXT("archive");
uint32 UMainSaveGame::UserIndex = 0;

static FORCEINLINE FByteArray GetObjectData(UObject* Obj)
{
	TArray<uint8> Data;
	// 创建写入器
	FMemoryWriter MemoryWriter(Data, true);
	FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
	Ar.ArIsSaveGame = true;
	Ar.ArNoDelta = true;
	Obj->Serialize(Ar);
	return Data;
}

static FORCEINLINE FActorData GetActorData(UObject* Obj)
{
	FActorData Data;
	(*Data).Add("Self", GetObjectData(Obj));
	for (TFieldIterator<FObjectProperty> It(Obj->GetClass(), EFieldIterationFlags::IncludeSuper); It; ++It)
	{
		auto Comp = (*It)->GetPropertyValue_InContainer(Obj);
		// 自定义的组名，避免无用的组件被保存下来
		if (Comp && Comp->GetClass()->IsClassGroupName(L"SaveGame"))
		{
			(*Data).Add(Comp->GetName(), GetObjectData(Comp));
		}
	}
	return Data;
}

static FORCEINLINE void SetObjectData(UObject* Obj, const FByteArray& Data)
{
	// 创建阅读器
	FMemoryReader MemoryReader(*Data, true);
	FObjectAndNameAsStringProxyArchive Ar(MemoryReader, false);
	Ar.ArIsSaveGame = true;
	Ar.ArNoDelta = true;
	Obj->Serialize(Ar);
}

static FORCEINLINE void SetActorData(UObject* Obj, const FActorData& Data)
{
	SetObjectData(Obj, *(*Data)["Self"]);
	for (TFieldIterator<FObjectProperty> It(Obj->GetClass(), EFieldIterationFlags::IncludeSuper); It; ++It)
	{
		auto Comp = (*It)->GetPropertyValue_InContainer(Obj);
		if (Comp && (*Data).Contains(Comp->GetName()))
		{
			SetObjectData(Comp, (*Data)[Comp->GetName()]);
		}
	}
}

UMainSaveGame::UMainSaveGame()
{
}

void UMainSaveGame::SaveAr()
{
	static bool IsBusy = false;
	if (IsBusy) return;

	IsBusy = true;
	TActorIterator<ACelestialBody> Planet_It(AMainLevelScript::Instance()->GetWorld());
	TActorIterator<AMovableMeshActor> Move_It(AMainLevelScript::Instance()->GetWorld());
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [Planet_It, Move_It] {
		if (UMainSaveGame* Instance = Cast<UMainSaveGame>(UGameplayStatics::CreateSaveGameObject(UMainSaveGame::StaticClass())))
		{
			// 保存星球的数据
			for (auto It = Planet_It; It; ++It)
			{
				// 每个星球都有独立的蓝图类，所以这里使用Class的路径名
				ACelestialBody* Planet = *It;
				Instance->PlanetList.Add({ Planet->GetClass()->GetPathName(), GetActorData(Planet) });
			}

			// 保存所有质量体的数据
			for (auto It = Move_It; It; ++It)
			{
				AMovableMeshActor* Actor = *It;
				Instance->MovableActorList.Add({ Actor->GetName(), GetActorData(Actor) });
			}

			//保存基地的数据
			Instance->EarthBase = GetActorData(AMainLevelScript::GetEarthBase());

			// 保存主角的数据（如有）
			if (auto MainChar = AMainLevelScript::GetMainChar())
			{
				Instance->MainChar =GetActorData(MainChar);
			}

			// 保存飞船的数据（如有）
			if (auto Spaceship = AMainLevelScript::GetSpaceship())
			{
				Instance->Spaceship = GetActorData(Spaceship);
			}

			// 保存等级状态
			int64 NowTimestamp = FDateTime::UtcNow().ToUnixTimestamp();
			AMainPlayerState::Instance()->UpdateTotalTime(NowTimestamp - SaveTimestamp);
			Instance->PlayerState = GetObjectData(AMainPlayerState::Instance());
			SaveTimestamp = NowTimestamp;

			UGameplayStatics::SaveGameToSlot(Instance, SaveSlotName, UserIndex);
		}
		IsBusy = false;
	});
}

void UMainSaveGame::LoadAr()
{
	if (UMainSaveGame* LoadedGame = Cast<UMainSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex)))
	{
		UMainSaveGame* Instance = Cast<UMainSaveGame>(LoadedGame);
		auto CurWorld = AMainLevelScript::Instance()->GetWorld();

		// 加载星球的数据
		for (TActorIterator<ACelestialBody> It(CurWorld); It; ++It)
		{
			ACelestialBody* Planet = *It;
			FString KeyName = Planet->GetClass()->GetPathName();
			if (Instance->PlanetList.Contains(KeyName))
			{
				SetActorData(Planet, Instance->PlanetList[KeyName]);
			}
		}

		// 加载所有质量体的数据
		for (TActorIterator<AMovableMeshActor> It(CurWorld); It; ++It)
		{
			AMovableMeshActor* Actor = *It;
			FString KeyName = Actor->GetName();
			if (Instance->MovableActorList.Contains(KeyName))
			{
				SetActorData(Actor, Instance->MovableActorList[KeyName]);
			}
		}

		// 加载基地的数据
		SetActorData(AMainLevelScript::GetEarthBase(), Instance->EarthBase);

		// 加载主角数据
		if (!(*Instance->MainChar).IsEmpty())
		{
			if (AMainLevelScript::GetMainChar() == nullptr)
			{
				AMainLevelScript::GetEarthBase()->CreateMainChar();
			}
			SetActorData(AMainLevelScript::GetMainChar(), Instance->MainChar);
		}

		// 加载飞船的数据
		if (!(*Instance->Spaceship).IsEmpty())
		{
			if (AMainLevelScript::GetSpaceship() == nullptr)
			{
				AMainLevelScript::GetEarthBase()->CreateSpaceship();
			}
			SetActorData(AMainLevelScript::GetSpaceship(), Instance->Spaceship);
		}

		// 加载等级状态
		SetObjectData(AMainPlayerState::Instance(), Instance->PlayerState);
	}
}

bool UMainSaveGame::HasAr()
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex);
}
