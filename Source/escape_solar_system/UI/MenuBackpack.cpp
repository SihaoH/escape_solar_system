// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBackpack.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "BackpackView.h"
#include "ItemDataObject.h"

void UMenuBackpack::NativePreConstruct()
{
	BpView_Body = Cast<UBackpackView>(GetWidgetFromName(TEXT("BpView_Body")));
	BpView_Base = Cast<UBackpackView>(GetWidgetFromName(TEXT("BpView_Base")));
	BpView_Ship = Cast<UBackpackView>(GetWidgetFromName(TEXT("BpView_Ship")));
	BpView_Body->OnItemClicked.AddUniqueDynamic(this, &UMenuBackpack::OnBpViewItemClicked);
	BpView_Base->OnItemClicked.AddUniqueDynamic(this, &UMenuBackpack::OnBpViewItemClicked);
	BpView_Ship->OnItemClicked.AddUniqueDynamic(this, &UMenuBackpack::OnBpViewItemClicked);

	return Super::NativePreConstruct();
}

void UMenuBackpack::NativeConstruct()
{
	AMainCharacter* MainCharacter = AMainCharacter::GetInstance();
	check(MainCharacter);
	AEarthBaseActor* EarthBase = MainCharacter->FindEarthBase();
	ASpaceship* Spaceship = MainCharacter->FindSpaceship();
	if (Spaceship == nullptr && EarthBase)
	{
		Spaceship = EarthBase->FindSpaceship();
	}

	BpView_Body->SetBackpack(MainCharacter->GetBackpack());
	BpView_Base->SetBackpack(EarthBase ? EarthBase->GetBackpack() : nullptr);
	BpView_Ship->SetBackpack(Spaceship ? Spaceship->GetBackpack() : nullptr);

	return Super::NativeConstruct();
}

void UMenuBackpack::OnBpViewItemClicked(UBackpackView* BpView, UObject* Item)
{
	if (BpView != BpView_Body)
	{
		BpView_Body->ClearSelection();
	}
	if (BpView != BpView_Base)
	{
		BpView_Base->ClearSelection();
	}
	if (BpView != BpView_Ship)
	{
		BpView_Ship->ClearSelection();
	}

	SelectItem(Cast<UItemDataObject>(Item));
}

void UMenuBackpack::SelectItem(UItemDataObject* Item)
{
	// TODO 展示选中物品的信息，以及丢弃功能
}
