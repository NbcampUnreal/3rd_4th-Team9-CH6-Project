// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/inventory/RSInventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ItemDataAsset/RSItemdata.h"


void URSInventorySlotWidget::SetData(URSItemData* InItemData, int32 InCount)
{
	ItemData = InItemData;
	Count = InCount;

	if (!ItemData) return;

	// Icon
	if (IconImage) // Icon 타입이 UTexture2D*라고 가정
	{
		UTexture2D* Tex = ItemData->Icon.Get(); // 로드 안 됐으면 nullptr
		if (Tex)
		{
			IconImage->SetBrushFromTexture(Tex);
		}
		
	}

	// Name
	if (NameText)
	{
		NameText->SetText(ItemData->DisplayName); // DisplayName 타입이 FText라고 가정
	}

	// Count
	if (CountText)
	{
		CountText->SetText(FText::AsNumber(Count));
		CountText->SetVisibility(Count > 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
