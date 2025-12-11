#pragma once

#include "Blueprint/UserWidget.h"
#include "RSEquipmentSlotWidget.generated.h"

class URSEquipmentEntryWidget;
class URSEquipmentManagerComponent;

UCLASS()
class URSEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSEquipmentSlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void OnDragEnded();

protected:
	UPROPERTY()
	TObjectPtr<URSEquipmentManagerComponent> EquipmentManager;

protected:
	bool bAlreadyHovered = false;
};
