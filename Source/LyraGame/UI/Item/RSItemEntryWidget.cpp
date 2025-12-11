#include "RSItemEntryWidget.h"

#include "Components/Image.h"
#include "Data/RSItemData.h"
#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "RSItemDragWidget.h"
#include "RSItemHoverWidget.h"
#include "Components/TextBlock.h"
#include "Data/RSUIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemEntryWidget)

URSItemEntryWidget::URSItemEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void URSItemEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Text_Count->SetText(FText::GetEmpty());
}

void URSItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Image_Hover->SetVisibility(ESlateVisibility::Hidden);
}

void URSItemEntryWidget::NativeDestruct()
{
	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
	
	Super::NativeDestruct();
}

void URSItemEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Image_Hover->SetVisibility(ESlateVisibility::Visible);

	if (HoverWidget == nullptr)
	{
		TSubclassOf<URSItemHoverWidget> HoverWidgetClass = URSUIData::Get().ItemHoverWidgetClass;
		HoverWidget = CreateWidget<URSItemHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
	}

	if (HoverWidget)
	{
		HoverWidget->RefreshUI(ItemInstance);
		HoverWidget->AddToViewport();
	}
}

FReply URSItemEntryWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	
	return Reply;
}

void URSItemEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Image_Hover->SetVisibility(ESlateVisibility::Hidden);

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
}

FReply URSItemEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Reply;
}

void URSItemEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	RefreshWidgetOpacity(false);
}

void URSItemEntryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	RefreshWidgetOpacity(true);
}

void URSItemEntryWidget::RefreshWidgetOpacity(bool bClearlyVisible)
{
	SetRenderOpacity(bClearlyVisible ? 1.f : 0.5f);
}

void URSItemEntryWidget::RefreshUI(URSItemInstance* NewItemInstance, int32 NewItemCount)
{
	if (NewItemInstance == nullptr || NewItemCount < 1)
		return;
	
	ItemInstance = NewItemInstance;
	ItemCount = NewItemCount;

	const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	Image_Icon->SetBrushFromTexture(ItemTemplate.IconTexture, true);
	Text_Count->SetText(ItemCount <= 1 ? FText::GetEmpty() : FText::AsNumber(ItemCount));

	UTexture2D* RarityTexture = URSUIData::Get().GetEntryRarityTexture(ItemInstance->GetItemRarity());
	Image_RarityCover->SetBrushFromTexture(RarityTexture, true);
}

void URSItemEntryWidget::RefreshItemCount(int32 NewItemCount)
{
	if (NewItemCount < 1)
		return;
	
	ItemCount = NewItemCount;
	Text_Count->SetText(ItemCount <= 1 ? FText::GetEmpty() : FText::AsNumber(ItemCount));
}
