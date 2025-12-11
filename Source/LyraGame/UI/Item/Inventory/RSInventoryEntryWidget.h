#pragma once

#include "UI/Item/RSItemEntryWidget.h"
#include "RSInventoryEntryWidget.generated.h"

class USizeBox;
class URSItemInstance;
class URSInventorySlotsWidget;

UCLASS()
class URSInventoryEntryWidget : public URSItemEntryWidget
{
	GENERATED_BODY()
	
public:
	URSInventoryEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(URSInventorySlotsWidget* InSlotsWidget, URSItemInstance* InItemInstance, int32 InItemCount);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

private:
	UPROPERTY()
	TObjectPtr<URSInventorySlotsWidget> SlotsWidget;

	FIntPoint CachedFromSlotPos = FIntPoint::ZeroValue;
	FVector2D CachedDeltaWidgetPos = FVector2D::ZeroVector;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
};
