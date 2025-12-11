#pragma once

#include "RSDefine.h"
#include "Blueprint/UserWidget.h"
#include "RSUtilitySlotWidget.generated.h"

class URSItemInstance;
class UTextBlock;
class UImage;
class URSEquipmentManagerComponent;
class URSEquipManagerComponent;

UCLASS()
class URSUtilitySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSUtilitySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance, int32 ItemCount);
	void OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState);
	
public:
	UPROPERTY(EditAnywhere)
	EUtilitySlotType WidgetUtilitySlotType = EUtilitySlotType::Count;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SlotNumber;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Highlight;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Count;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_Highlight_In;
	
private:
	UPROPERTY()
	TObjectPtr<URSEquipmentManagerComponent> EquipmentManager;
	
	UPROPERTY()
	TObjectPtr<URSEquipManagerComponent> EquipManager;
	
private:
	FDelegateHandle EntryChangedDelegateHandle;
	FDelegateHandle EquipStateChangedDelegateHandle;
};
