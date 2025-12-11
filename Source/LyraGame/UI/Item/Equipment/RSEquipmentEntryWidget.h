#pragma once

#include "RSDefine.h"
#include "UI/Item/RSItemEntryWidget.h"
#include "RSEquipmentEntryWidget.generated.h"

class URSItemInstance;
class URSEquipmentManagerComponent;

UCLASS()
class URSEquipmentEntryWidget : public URSItemEntryWidget
{
	GENERATED_BODY()
	
public:
	URSEquipmentEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(URSItemInstance* InItemInstance, int32 InItemCount, EEquipmentSlotType InEquipmentSlotType, URSEquipmentManagerComponent* InEquipmentManager);
	
protected:
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY()
	TObjectPtr<URSEquipmentManagerComponent> EquipmentManager;

	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
};
