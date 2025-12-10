#include "RSInventoryStatWidget.h"

#include "Data/RSUIData.h"
#include "UI/Item/RSSkillStatHoverWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSInventoryStatWidget)

URSInventoryStatWidget::URSInventoryStatWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSInventoryStatWidget::NativeDestruct()
{
	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
	
	Super::NativeDestruct();
}

void URSInventoryStatWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	const FRSUIInfo& UIInfo = URSUIData::Get().GetTagUIInfo(StatTag);
	if (UIInfo.Title.IsEmpty() == false && UIInfo.Content.IsEmpty() == false)
	{
		if (HoverWidget == nullptr)
		{
			TSubclassOf<URSSkillStatHoverWidget> HoverWidgetClass = URSUIData::Get().SkillStatHoverWidget;
			HoverWidget = CreateWidget<URSSkillStatHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
		}

		if (HoverWidget)
		{
		
			HoverWidget->RefreshUI(UIInfo.Title, UIInfo.Content);
			HoverWidget->AddToViewport();
		}
	}
}

FReply URSInventoryStatWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	
	return Reply;
}

void URSInventoryStatWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
}
