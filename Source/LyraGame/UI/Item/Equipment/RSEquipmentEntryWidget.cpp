#include "RSEquipmentEntryWidget.h"

#include "RSEquipmentSlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Data/RSItemData.h"
#include "Data/RSUIData.h"
#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "UI/Item/RSItemDragDrop.h"
#include "UI/Item/RSItemDragWidget.h"
#include "Item/Managers/RSItemManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipmentEntryWidget)

URSEquipmentEntryWidget::URSEquipmentEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSEquipmentEntryWidget::Init(URSItemInstance* InItemInstance, int32 InItemCount, EEquipmentSlotType InEquipmentSlotType, URSEquipmentManagerComponent* InEquipmentManager)
{
	if (InEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || InEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || InEquipmentSlotType == EEquipmentSlotType::Count)
		return;
	
	RefreshUI(InItemInstance, InItemCount);
	
	EquipmentSlotType = InEquipmentSlotType;
	EquipmentManager = InEquipmentManager;
}

void URSEquipmentEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	FIntPoint UnitInventorySlotSize = URSUIData::Get().UnitInventorySlotSize;
	const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

	TSubclassOf<URSItemDragWidget> DragWidgetClass = URSUIData::Get().DragWidgetClass;
	URSItemDragWidget* ItemDragWidget = CreateWidget<URSItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	FVector2D DragWidgetSize = FVector2D(ItemTemplate.SlotCount * UnitInventorySlotSize);
	ItemDragWidget->Init(DragWidgetSize, ItemTemplate.IconTexture, ItemCount);
	
	URSItemDragDrop* ItemDragDrop = NewObject<URSItemDragDrop>();
	ItemDragDrop->DefaultDragVisual = ItemDragWidget;
	ItemDragDrop->Pivot = EDragPivot::CenterCenter;
	ItemDragDrop->FromEntryWidget = this;
	ItemDragDrop->FromEquipmentManager = EquipmentManager;
	ItemDragDrop->FromEquipmentSlotType = EquipmentSlotType;
	ItemDragDrop->FromItemInstance = ItemInstance;
	ItemDragDrop->DeltaWidgetPos = (DragWidgetSize / 2.f) - (UnitInventorySlotSize / 2.f);
	OutOperation = ItemDragDrop;
}

FReply URSEquipmentEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	if (Reply.IsEventHandled() == false && UWidgetBlueprintLibrary::IsDragDropping() == false && InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (URSItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<URSItemManagerComponent>())
		{
			ItemManager->Server_QuickFromEquipment(EquipmentManager, EquipmentSlotType);
			return FReply::Handled();
		}
	}

	return Reply;
}
