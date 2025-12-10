#pragma once

#include "Blueprint/UserWidget.h"
#include "RSItemDropWidget.generated.h"

UCLASS()
class URSItemDropWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSItemDropWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
