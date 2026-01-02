// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/Inventory/RSInventoryType.h"
#include "RSInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class URSInventoryComponent;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlotUseRequested, int32 /*SlotIndex*/, UUserWidget* /*Sender*/);

UCLASS()
class REMNANTSOUL_API URSInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
	


public:
	void Setup(URSInventoryComponent* InInv, int32 InIndex);
	void SetItem(const FInventoryItem& InItem);

	UFUNCTION()
	void ClearSlot();

	FOnSlotUseRequested OnUseRequested;
protected:
	
	// 우클릭 / 더블클릭 처리
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CountText;

private:
	UPROPERTY()
	TObjectPtr<URSInventoryComponent> InventoryComp;
	int32 SlotIndex = INDEX_NONE;
	FInventoryItem Item;
};
