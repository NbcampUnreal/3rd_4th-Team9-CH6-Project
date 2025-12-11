#include "RSEquipmentSlotWidget.h"

#include "RSEquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipmentSlotWidget)

URSEquipmentSlotWidget::URSEquipmentSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSEquipmentSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	OnDragEnded();
}

void URSEquipmentSlotWidget::OnDragEnded()
{
	bAlreadyHovered = false;
}
