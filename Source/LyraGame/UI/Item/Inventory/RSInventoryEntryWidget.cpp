#include "RSInventoryEntryWidget.h"

#include "RSInventorySlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SizeBox.h"
#include "Data/RSItemData.h"
#include "Data/RSUIData.h"
#include "Item/RSItemInstance.h"
#include "Item/Fragments/RSItemFragment_Equipable.h"
#include "Item/Managers/RSInventoryManagerComponent.h"
#include "Item/Managers/RSItemManagerComponent.h"
#include "UI/Item/RSItemDragDrop.h"
#include "UI/Item/RSItemDragWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSInventoryEntryWidget)

URSInventoryEntryWidget::URSInventoryEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSInventoryEntryWidget::Init(URSInventorySlotsWidget* InSlotsWidget, URSItemInstance* InItemInstance, int32 InItemCount)
{
	if (InSlotsWidget == nullptr || InItemInstance == nullptr)
		return;
	
	SlotsWidget = InSlotsWidget;

	FIntPoint UnitInventorySlotSize = URSUIData::Get().UnitInventorySlotSize;
	const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(InItemInstance->GetItemTemplateID());

	FVector2D WidgetSize = FVector2D(ItemTemplate.SlotCount.X * UnitInventorySlotSize.X, ItemTemplate.SlotCount.Y * UnitInventorySlotSize.Y);
	SizeBox_Root->SetWidthOverride(WidgetSize.X);
	SizeBox_Root->SetHeightOverride(WidgetSize.Y);
	
	RefreshUI(InItemInstance, InItemCount);
}

FReply URSInventoryEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	FIntPoint UnitInventorySlotSize = URSUIData::Get().UnitInventorySlotSize;
	
	FVector2D MouseWidgetPos = SlotsWidget->GetSlotContainerGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D ItemWidgetPos = SlotsWidget->GetSlotContainerGeometry().AbsoluteToLocal(InGeometry.LocalToAbsolute(UnitInventorySlotSize / 2.f));
	FIntPoint ItemSlotPos = FIntPoint(ItemWidgetPos.X / UnitInventorySlotSize.X, ItemWidgetPos.Y / UnitInventorySlotSize.Y);
	
	CachedFromSlotPos = ItemSlotPos;
	CachedDeltaWidgetPos = MouseWidgetPos - ItemWidgetPos;
	
	if (Reply.IsEventHandled() == false && UWidgetBlueprintLibrary::IsDragDropping() == false && InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		URSItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<URSItemManagerComponent>();
		URSInventoryManagerComponent* FromInventoryManager = SlotsWidget->GetInventoryManager();

		if (ItemManager && FromInventoryManager)
		{
			ItemManager->Server_QuickFromInventory(FromInventoryManager, ItemSlotPos);
			return FReply::Handled();
		}
	}
	
	return Reply;
}

void URSInventoryEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	FIntPoint UnitInventorySlotSize = URSUIData::Get().UnitInventorySlotSize;
	const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

	TSubclassOf<URSItemDragWidget> DragWidgetClass = URSUIData::Get().DragWidgetClass;
	URSItemDragWidget* DragWidget = CreateWidget<URSItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	FVector2D DragWidgetSize = FVector2D(ItemTemplate.SlotCount.X * UnitInventorySlotSize.X, ItemTemplate.SlotCount.Y * UnitInventorySlotSize.Y);
	DragWidget->Init(DragWidgetSize, ItemTemplate.IconTexture, ItemCount);
	
	URSItemDragDrop* DragDrop = NewObject<URSItemDragDrop>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::TopLeft;
	DragDrop->Offset = -((CachedDeltaWidgetPos + UnitInventorySlotSize / 2.f) / DragWidgetSize);
	DragDrop->FromEntryWidget = this;
	DragDrop->FromInventoryManager = SlotsWidget->GetInventoryManager();
	DragDrop->FromItemSlotPos = CachedFromSlotPos;
	DragDrop->FromItemInstance = ItemInstance;
	DragDrop->DeltaWidgetPos = CachedDeltaWidgetPos;
	OutOperation = DragDrop;
}
