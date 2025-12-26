// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/inventory/RSInventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "UObject/SoftObjectPtr.h"
#include "ItemDataAsset/RSItemdata.h"


void URSInventorySlotWidget::Setup(URSInventoryComponent* InInv, int32 InIndex)
{
	InventoryComp = InInv;
	SlotIndex = InIndex;
}

void URSInventorySlotWidget::SetItem(const FInventoryItem& InItem)
{
	Item = InItem;

	UTexture2D* Tex = Item.ItemData ? Item.ItemData->Icon.Get() : nullptr;
	if (!Tex && Item.ItemData)
	{
		Tex = Item.ItemData->Icon.LoadSynchronous();
	}

	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(Tex, true);
		ItemIcon->SetVisibility(Tex ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	// 2) 수량
	if (CountText)
	{
		const int32 Count = Item.Count;
		CountText->SetText(FText::AsNumber(Count));
		CountText->SetVisibility(Count > 1 ? ESlateVisibility::HitTestInvisible
										   : ESlateVisibility::Collapsed);
	}
}

void URSInventorySlotWidget::ClearSlot()
{ 
	Item = FInventoryItem{}; // 또는 bHasItem=false 같은 플래그

	if (ItemIcon)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (CountText)
	{
		CountText->SetVisibility(ESlateVisibility::Collapsed);
		CountText->SetText(FText::GetEmpty());
	}
}
