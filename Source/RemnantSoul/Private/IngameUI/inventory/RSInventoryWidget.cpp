// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/inventory/RSInventoryWidget.h"
#include "Components/PanelWidget.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "Component/Inventory/RSInventoryType.h"
#include "IngameUI/inventory/RSInventorySlotWidget.h"
#include "ItemDataAsset/RSItemData.h"

void URSInventoryWidget::Init(URSInventoryComponent* InInventory)
{
	UE_LOG(LogTemp, Log, TEXT("[InvUI] Init called. InInventory=%s"), *GetNameSafe(InInventory));

	InventoryComp = InInventory;
	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InvUI] InventoryComp is null"));
		return;
	}

	// (Dynamic delegate면 AddDynamic, Native delegate면 AddUObject)
	UE_LOG(LogTemp, Log, TEXT("[InvUI] Bind OnInventoryChanged"));
	InventoryComp->OnInventoryChanged.AddUObject(this, &ThisClass::Refresh);

	UE_LOG(LogTemp, Log, TEXT("[InvUI] First Refresh"));
	Refresh();
}



void URSInventoryWidget::Refresh()
{
	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InvUI] InventoryComp null"));
		return;
	}

	if (SlotWidgets.Num() == 0)
	{
		CollectSlots(); // 최초 1회 or 안전장치
	}

	// 1) 일단 전부 비우기
	for (URSInventorySlotWidget* Slots : SlotWidgets)
	{
		if (Slots) Slots->ClearSlot();
	}

	// 2) 아이템을 슬롯에 채우기
	const TArray<FInventoryItem>& Items = InventoryComp->GetItems();

	const int32 FillCount = FMath::Min(Items.Num(), SlotWidgets.Num());
	for (int32 i = 0; i < FillCount; ++i)
	{
		if (SlotWidgets[i])
		{
			SlotWidgets[i]->SetItem(Items[i]);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[InvUI] Refresh done. Items=%d Slots=%d"),
		Items.Num(), SlotWidgets.Num());
}


void URSInventoryWidget::CollectSlots()
{
	SlotWidgets.Reset();

	if (!SlotContainer)
	{
		UE_LOG(LogTemp, Error, TEXT("[InvUI] SlotContainer is null"));
		return;
	}

	const int32 ChildNum = SlotContainer->GetChildrenCount();
	for (int32 i = 0; i < ChildNum; ++i)
	{
		if (UWidget* Child = SlotContainer->GetChildAt(i))
		{
			if (URSInventorySlotWidget* Slots = Cast<URSInventorySlotWidget>(Child))
			{
				SlotWidgets.Add(Slots);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[InvUI] Collected Slots=%d"), SlotWidgets.Num());
}