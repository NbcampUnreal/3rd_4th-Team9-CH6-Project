#pragma once

#include "Item/RSItemInstance.h"
#include "UI/RSHoverWidget.h"
#include "RSItemHoverWidget.generated.h"

class URSEquipmentManagerComponent;
class URSItemInstance;
class UCanvasPanel;
class UHorizontalBox;
class URSItemHoverEntryWidget;

UCLASS()
class URSItemHoverWidget : public URSHoverWidget
{
	GENERATED_BODY()
	
public:
	URSItemHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(const URSItemInstance* HoveredItemInstance);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSItemHoverEntryWidget> HoverWidget_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSItemHoverEntryWidget> HoverWidget_Right;
};
