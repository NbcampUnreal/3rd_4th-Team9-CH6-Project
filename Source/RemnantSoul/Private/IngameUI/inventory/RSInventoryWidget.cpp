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
	UE_LOG(LogTemp, Log, TEXT("[InvUI] Init called. InInventory=%s"), *GetNameSafe(InInventory));

	InventoryComp = InInventory;
	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InvUI] InventoryComp is null"));
		return;
	}

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
		CollectSlots(); // 여기서 필요 시 자동 생성까지 됨
	}

	// 1) 전부 비우기
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

	// 1) 디자이너에 이미 슬롯들이 배치돼 있으면: 기존 방식 그대로 수집
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

	// 2) 자식 슬롯이 없다면: 자동 생성/배치
	if (SlotWidgets.Num() == 0 && bAutoBuildSlots)
	{
		BuildSlots();
	}

	// 3) 인덱스/인벤토리 포인터 세팅 (기존 슬롯이든 자동 생성이든 동일하게)
	for (int32 i = 0; i < SlotWidgets.Num(); ++i)
	{
		if (SlotWidgets[i])
		{
			SlotWidgets[i]->Setup(InventoryComp, i);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[InvUI] Collected/Created Slots=%d"), SlotWidgets.Num());
}

void URSInventoryWidget::BuildSlots()
{
	if (!SlotContainer)
	{
		UE_LOG(LogTemp, Error, TEXT("[InvUI] BuildSlots: SlotContainer is null"));
		return;
	}
	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[InvUI] BuildSlots: SlotWidgetClass is null (assign in BP)"));
		return;
	}

	const int32 TotalSlots = FMath::Max(0, Rows * Columns);

	// 컨테이너 비우고 새로 생성
	SlotContainer->ClearChildren();
	SlotWidgets.Reset();
	SlotWidgets.Reserve(TotalSlots);

	// UniformGridPanel이면 행/열 자동 배치
	if (UUniformGridPanel* Grid = Cast<UUniformGridPanel>(SlotContainer))
	{
		for (int32 Index = 0; Index < TotalSlots; ++Index)
		{
			const int32 Row = Index / Columns;
			const int32 Col = Index % Columns;

			URSInventorySlotWidget* Slots = CreateWidget<URSInventorySlotWidget>(GetWorld(), SlotWidgetClass);
			if (!Slots) continue;

			// Grid에 추가 + Row/Col 지정
			if (UUniformGridSlot* GridSlot = Grid->AddChildToUniformGrid(Slots, Row, Col))
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
				
			}

			SlotWidgets.Add(Slots);
		}

		UE_LOG(LogTemp, Log, TEXT("[InvUI] BuildSlots: UniformGrid %dx%d -> %d slots"),
			Rows, Columns, TotalSlots);
		return;
	}

	// UniformGridPanel이 아닌 경우(VerticalBox/WrapBox 등): 그냥 순서대로 AddChild
	for (int32 Index = 0; Index < TotalSlots; ++Index)
	{
		URSInventorySlotWidget* Slots = CreateWidget<URSInventorySlotWidget>(GetWorld(), SlotWidgetClass);
		if (!Slots) continue;

		SlotContainer->AddChild(Slots);
		SlotWidgets.Add(Slots);
	}

	UE_LOG(LogTemp, Log, TEXT("[InvUI] BuildSlots: Non-grid panel -> %d slots"), TotalSlots);
}