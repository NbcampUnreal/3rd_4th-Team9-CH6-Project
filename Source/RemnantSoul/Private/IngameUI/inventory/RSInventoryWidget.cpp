// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/inventory/RSInventoryWidget.h"

#include "Components/PanelWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "Component/Inventory/RSInventoryType.h"
#include "IngameUI/inventory/RSInventorySlotWidget.h"

void URSInventoryWidget::Init(URSInventoryComponent* InInventory)
{
	InventoryComp = InInventory;
	if (!InventoryComp)
	{
		return;
	}

	if (!InventoryChangedHandle.IsValid())
	{
		InventoryChangedHandle = InventoryComp->OnInventoryChanged.AddUObject(this, &ThisClass::Refresh);
	}

	Refresh();
}

void URSInventoryWidget::NativeDestruct()
{
	if (InventoryComp && InventoryChangedHandle.IsValid())
	{
		InventoryComp->OnInventoryChanged.Remove(InventoryChangedHandle);
		InventoryChangedHandle.Reset();
	}

	Super::NativeDestruct();
}

void URSInventoryWidget::Refresh()
{
	if (!InventoryComp)
	{
		return;
	}

	const int32 DesiredSlots = InventoryComp->GetSlotCount();
	if (SlotWidgets.Num() == 0 || (DesiredSlots > 0 && SlotWidgets.Num() != DesiredSlots))
	{
		CollectSlots();
	}

	for (URSInventorySlotWidget* SlotWidget : SlotWidgets)
	{
		if (SlotWidget)
		{
			SlotWidget->ClearSlot();
		}
	}

	const TArray<FInventoryItem>& Slots = InventoryComp->GetSlots();
	const int32 FillCount = FMath::Min(Slots.Num(), SlotWidgets.Num());

	for (int32 i = 0; i < FillCount; ++i)
	{
		if (SlotWidgets[i])
		{
			SlotWidgets[i]->SetItem(Slots[i]);
		}
	}
}

void URSInventoryWidget::CollectSlots()
{
	SlotWidgets.Reset();

	if (!SlotContainer)
	{
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

	if (SlotWidgets.Num() == 0 && bAutoBuildSlots)
	{
		BuildSlots();
	}

	for (int32 i = 0; i < SlotWidgets.Num(); ++i)
	{
		if (SlotWidgets[i])
		{
			SlotWidgets[i]->Setup(InventoryComp, i);
		}
	}
}

void URSInventoryWidget::BuildSlots()
{
	if (!SlotContainer || !SlotWidgetClass)
	{
		return;
	}

	const int32 InventorySlots = InventoryComp ? InventoryComp->GetSlotCount() : 0;
	const int32 TotalSlots = (InventorySlots > 0) ? InventorySlots : FMath::Max(0, Rows * Columns);

	SlotContainer->ClearChildren();
	SlotWidgets.Reset();
	SlotWidgets.Reserve(TotalSlots);

	if (UUniformGridPanel* Grid = Cast<UUniformGridPanel>(SlotContainer))
	{
		for (int32 Index = 0; Index < TotalSlots; ++Index)
		{
			URSInventorySlotWidget* Slots = CreateWidget<URSInventorySlotWidget>(GetWorld(), SlotWidgetClass);
			if (!Slots)
			{
				continue;
			}

			const int32 Row = Index / Columns;
			const int32 Col = Index % Columns;

			if (UUniformGridSlot* GridSlot = Grid->AddChildToUniformGrid(Slots, Row, Col))
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}

			SlotWidgets.Add(Slots);
		}
	}
	else
	{
		for (int32 Index = 0; Index < TotalSlots; ++Index)
		{
			URSInventorySlotWidget* Slots = CreateWidget<URSInventorySlotWidget>(GetWorld(), SlotWidgetClass);
			if (!Slots)
			{
				continue;
			}

			SlotContainer->AddChild(Slots);
			SlotWidgets.Add(Slots);
		}
	}
	
	int32 BoundCount = 0;

	for (URSInventorySlotWidget* Slots : SlotWidgets)
	{
		if (!Slots)
		{
			continue;
		}

		Slots->OnUseRequested.AddUObject(this, &ThisClass::HandleSlotUseRequested);
		++BoundCount;
	}

	
}

void URSInventoryWidget::HandleSlotUseRequested(int32 SlotIndex, UUserWidget* Sender)
{


	OnUseRequested.Broadcast(SlotIndex);
	
	
}
