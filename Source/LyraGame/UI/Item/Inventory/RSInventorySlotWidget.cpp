#include "RSInventorySlotWidget.h"

#include "Components/SizeBox.h"
#include "Data/RSUIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSInventorySlotWidget)

URSInventorySlotWidget::URSInventorySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSInventorySlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FIntPoint UnitInventorySlotSize = URSUIData::Get().UnitInventorySlotSize;
	SizeBox_Root->SetWidthOverride(UnitInventorySlotSize.X);
	SizeBox_Root->SetHeightOverride(UnitInventorySlotSize.Y);
}
