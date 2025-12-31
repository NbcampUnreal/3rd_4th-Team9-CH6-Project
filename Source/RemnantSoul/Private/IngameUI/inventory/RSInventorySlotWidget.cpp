// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/inventory/RSInventorySlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "ItemDataAsset/RSItemData.h"

void URSInventorySlotWidget::Setup(URSInventoryComponent* InInv, int32 InIndex)
{
	InventoryComp = InInv;
	SlotIndex = InIndex;
}

void URSInventorySlotWidget::SetItem(const FInventoryItem& InItem)
{
	Item = InItem;

	UTexture2D* Tex = nullptr;
	if (Item.ItemData)
	{
		Tex = Item.ItemData->Icon.Get();
		if (!Tex && !Item.ItemData->Icon.IsNull())
		{
			// 간단 구현: 성능 필요하면 StreamableManager로 비동기 로드로 교체
			Tex = Item.ItemData->Icon.LoadSynchronous();
		}
	}

	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(Tex, true);
		ItemIcon->SetVisibility(Tex ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (CountText)
	{
		const int32 Count = Item.Count;
		CountText->SetText(FText::AsNumber(Count));
		CountText->SetVisibility(Count > 1 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void URSInventorySlotWidget::ClearSlot()
{
	Item = FInventoryItem{};

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

FReply URSInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 슬롯이 비어있으면 무시
	if (Item.IsEmpty() || SlotIndex == INDEX_NONE)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	const bool bRightClick = InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
	//const bool bDoubleClick = InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && InMouseEvent.GetClickCount() >= 2;

	if (bRightClick)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Slot] Broadcast UseRequested. SlotIndex=%d Sender=%s"),
		SlotIndex, *GetNameSafe(this));
		OnUseRequested.Broadcast(SlotIndex, this);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
