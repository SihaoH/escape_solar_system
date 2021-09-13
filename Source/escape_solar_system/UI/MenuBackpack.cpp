// Copyright 2020 H₂S. All Rights Reserved.

#include "MenuBackpack.h"
#include "MainCharacter.h"
#include "Spaceship.h"
#include "EarthBaseActor.h"
#include "BackpackView.h"
#include "ItemDataObject.h"
#include "MainFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "MenuBackpack"

void UMenuBackpack::NativePreConstruct()
{
	ItemIcon.ImageSize = FVector2D(64.f, 64.f);

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
	// TODO 丢弃的功能
	NoneVisibility = Item ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	ItemVisibility = Item ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	if (Item)
	{
		FBasicItemData& Data = UMainFunctionLibrary::GetBasicItemData(Item->RowName);
		ItemIcon.SetResourceObject(Data.Icon.LoadSynchronous());
		ItemName = Data.Name;
		ItemDesc = Data.Desc;
		ItemMass = FText::Format(LOCTEXT("Mass", "重量: {0}KG"), Data.Mass);
		ItemStack = FText::Format(LOCTEXT("Stack", "最大堆叠数: {0}"), Data.MaxStack);
		ItemCount = FText::Format(LOCTEXT("Count", "当前物品槽: {0}"), Item->Count);
	}
}

#undef LOCTEXT_NAMESPACE
