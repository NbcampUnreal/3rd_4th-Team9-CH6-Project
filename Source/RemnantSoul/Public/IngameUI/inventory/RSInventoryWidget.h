// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RSInventoryWidget.generated.h"


class UPanelWidget;
class URSInventoryComponent;
class URSInventorySlotWidget;
/**
 * 
 */
UCLASS()
class REMNANTSOUL_API URSInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	//virtual void NativeConstruct() override;

	UFUNCTION()
	void Refresh();
	void Init(URSInventoryComponent* InInventory);
	void CollectSlots();
	

protected:
	// 슬롯들이 들어갈 컨테이너(VerticalBox든 UniformGrid든)
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UPanelWidget> SlotContainer;

	UPROPERTY()
	TArray<URSInventorySlotWidget*> SlotWidgets;
	
	// 슬롯 위젯 클래스 (WBP_InventorySlot을 지정)
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<URSInventorySlotWidget> SlotWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<URSInventoryComponent> InventoryComp;
};
