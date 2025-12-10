#include "RSInventoryValidWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Data/RSUIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSInventoryValidWidget)

URSInventoryValidWidget::URSInventoryValidWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSInventoryValidWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FIntPoint UnitInventorySlotSize = URSUIData::Get().UnitInventorySlotSize;
	SizeBox_Root->SetWidthOverride(UnitInventorySlotSize.X);
	SizeBox_Root->SetHeightOverride(UnitInventorySlotSize.Y);
}

void URSInventoryValidWidget::ChangeSlotState(ESlotState SlotState)
{
	switch (SlotState)
	{
	case ESlotState::Default:
		Image_Red->SetVisibility(ESlateVisibility::Hidden);
		Image_Green->SetVisibility(ESlateVisibility::Hidden);
		break;
	case ESlotState::Invalid:
		Image_Red->SetVisibility(ESlateVisibility::Visible);
		Image_Green->SetVisibility(ESlateVisibility::Hidden);
		break;
	case ESlotState::Valid:
		Image_Red->SetVisibility(ESlateVisibility::Hidden);
		Image_Green->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}
