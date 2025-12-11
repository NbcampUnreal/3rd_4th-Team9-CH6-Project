#include "RSItemDropWidget.h"

#include "RSItemDragDrop.h"
#include "RSItemEntryWidget.h"
#include "Item/Managers/RSItemManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemDropWidget)

URSItemDropWidget::URSItemDropWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

bool URSItemDropWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (URSItemDragDrop* ItemDragDrop = Cast<URSItemDragDrop>(InOperation))
	{
		if (URSItemEntryWidget* FromEntryWidget = ItemDragDrop->FromEntryWidget)
		{
			FromEntryWidget->RefreshWidgetOpacity(true);
		}

		if (URSItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<URSItemManagerComponent>())
		{
			if (ItemDragDrop->FromInventoryManager)
			{
				ItemManager->Server_DropItemFromInventory(ItemDragDrop->FromInventoryManager, ItemDragDrop->FromItemSlotPos);
			}
			else if (ItemDragDrop->FromEquipmentManager)
			{
				ItemManager->Server_DropItemFromEquipment(ItemDragDrop->FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType);
			}
		}
	}

	return true;
}
