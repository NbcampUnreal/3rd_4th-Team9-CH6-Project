// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/inventory/RSInventoryWidget.h"
#include "Components/PanelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "Component/Inventory/RSInventoryType.h"
#include "IngameUI/inventory/RSInventorySlotWidget.h"

void URSInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* Pawn = GetOwningPlayerPawn();
	if (!Pawn) return;

	InventoryComp = Pawn->FindComponentByClass<URSInventoryComponent>();
	if (!InventoryComp) return;

	// 인벤토리 변경 이벤트 바인딩
	InventoryComp->OnInventoryChanged.AddDynamic(this, &ThisClass::Refresh);

	Refresh();
}

void URSInventoryWidget::Refresh()
{
	if (!SlotContainer || !InventoryComp || !SlotWidgetClass) return;

	SlotContainer->ClearChildren();

	const TArray<FInventoryItem>& Items = InventoryComp->GetItems();

	for (const FInventoryItem& It : Items)
	{
		if (!It.ItemData || It.Count <= 0) continue;

		URSInventorySlotWidget* ItemSlot = CreateWidget<URSInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
		if (!ItemSlot) continue;

		ItemSlot->SetData(It.ItemData, It.Count);
		SlotContainer->AddChild(ItemSlot);
	}
}