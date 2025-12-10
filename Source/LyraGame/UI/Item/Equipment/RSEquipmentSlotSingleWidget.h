#pragma once

#include "RSDefine.h"
#include "RSEquipmentSlotWidget.h"
#include "RSEquipmentSlotSingleWidget.generated.h"

class UImage;
class UOverlay;
class URSItemInstance;
class URSEquipmentEntryWidget;
class URSEquipmentManagerComponent;

UENUM()
enum class EEquipmentSingleSlotType : uint8
{
	None,
	Armor,
	Utility,
};

UCLASS()
class URSEquipmentSlotSingleWidget : public URSEquipmentSlotWidget
{
	GENERATED_BODY()
	
public:
	URSEquipmentSlotSingleWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(EArmorType InArmorType, URSEquipmentManagerComponent* InEquipmentManager);
	void Init(EUtilitySlotType InUtilitySlotType, URSEquipmentManagerComponent* InEquipmentManager);
	
protected:
	virtual void NativePreConstruct() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void OnDragEnded() override;

public:
	void OnEquipmentEntryChanged(URSItemInstance* InItemInstance, int32 InItemCount);

private:
	EEquipmentSlotType GetEquipmentSlotType() const;
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> BaseIconTexture;
	
private:
	UPROPERTY()
	TObjectPtr<URSEquipmentEntryWidget> EntryWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Entry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_BaseIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green;

private:
	EEquipmentSingleSlotType EquipmentSingleSlotType = EEquipmentSingleSlotType::None;
	EArmorType ArmorType = EArmorType::Count;
	EUtilitySlotType UtilitySlotType = EUtilitySlotType::Count;
};
