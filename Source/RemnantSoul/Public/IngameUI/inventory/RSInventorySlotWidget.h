// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component//Inventory/RSInventoryType.h"
#include "RSInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class URSItemData;
class URSInventoryComponent;

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API URSInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void Setup(URSInventoryComponent* InInv, int32 InIndex);
	void SetItem(const FInventoryItem& InItem);
	UFUNCTION()
	void ClearSlot();

protected:
	//virtual void NativeConstruct() override;

	//UFUNCTION()
	//void OnClicked();

	UPROPERTY(meta=(BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CountText;

private:
	URSInventoryComponent* InventoryComp;
	int32 SlotIndex = INDEX_NONE;
	FInventoryItem Item;
};
