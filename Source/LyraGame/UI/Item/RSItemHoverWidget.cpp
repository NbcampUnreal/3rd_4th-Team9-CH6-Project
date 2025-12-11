#include "RSItemHoverWidget.h"

#include "RSItemHoverEntryWidget.h"
#include "Item/RSItemInstance.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemHoverWidget)

URSItemHoverWidget::URSItemHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSItemHoverWidget::RefreshUI(const URSItemInstance* HoveredItemInstance)
{
	const URSItemInstance* PairItemInstance = nullptr;
	if (URSEquipmentManagerComponent* EquipmentManager = GetOwningPlayerPawn()->GetComponentByClass<URSEquipmentManagerComponent>())
	{
		EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
		PairItemInstance = EquipmentManager->FindPairItemInstance(HoveredItemInstance, EquipmentSlotType);
	}
	
	HoverWidget_Left->RefreshUI(HoveredItemInstance);
	HoverWidget_Right->RefreshUI(PairItemInstance);
}
