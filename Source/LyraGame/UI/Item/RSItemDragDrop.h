#pragma once

#include "RSDefine.h"
#include "Blueprint/DragDropOperation.h"
#include "RSItemDragDrop.generated.h"

class URSItemInstance;
class URSItemEntryWidget;
class URSEquipmentManagerComponent;
class URSInventoryManagerComponent;

UCLASS()
class URSItemDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	URSItemDragDrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY()
	TObjectPtr<URSInventoryManagerComponent> FromInventoryManager;

	FIntPoint FromItemSlotPos = FIntPoint::ZeroValue;

public:
	UPROPERTY()
	TObjectPtr<URSEquipmentManagerComponent> FromEquipmentManager;

	EEquipmentSlotType FromEquipmentSlotType = EEquipmentSlotType::Count;

public:
	UPROPERTY()
	TObjectPtr<URSInventoryManagerComponent> ToInventoryManager;

	FIntPoint ToItemSlotPos = FIntPoint::ZeroValue;

public:
	UPROPERTY()
	TObjectPtr<URSEquipmentManagerComponent> ToEquipmentManager;

	EEquipmentSlotType ToEquipmentSlotType = EEquipmentSlotType::Count;
	
public:
	UPROPERTY()
	TObjectPtr<URSItemEntryWidget> FromEntryWidget;
	
	UPROPERTY()
	TObjectPtr<URSItemInstance> FromItemInstance;
	
	FVector2D DeltaWidgetPos = FVector2D::ZeroVector;
};
