// RSQuickSlotWidget.cpp
#include "IngameUI/inventory//RSQuickSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ItemDataAsset/RSItemData.h"

void URSQuickSlotWidget::SetItem(const FInventoryItem& InItem)
{
	UTexture2D* Tex = nullptr;

	if (InItem.ItemData)
	{
		Tex = InItem.ItemData->Icon.Get();
		if (!Tex && !InItem.ItemData->Icon.IsNull())
		{
			Tex = InItem.ItemData->Icon.LoadSynchronous();
		}
	}

	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(Tex, true);
		ItemIcon->SetVisibility(Tex ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (CountText)
	{
		CountText->SetText(FText::AsNumber(InItem.Count));
		CountText->SetVisibility(InItem.Count > 1 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void URSQuickSlotWidget::SetEmpty()
{
	if (ItemIcon) ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	if (CountText) CountText->SetVisibility(ESlateVisibility::Collapsed);
}
