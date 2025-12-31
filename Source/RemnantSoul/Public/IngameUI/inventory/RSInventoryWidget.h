// RSInventoryWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RSInventoryWidget.generated.h"
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryUseRequested, int32 /*SlotIndex*/);
class UPanelWidget;
class URSInventoryComponent;
class URSInventorySlotWidget;

UCLASS()
class REMNANTSOUL_API URSInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void Refresh();

	FOnInventoryUseRequested OnUseRequested;
	
	void Init(URSInventoryComponent* InInventory);
	void CollectSlots();
	void BuildSlots();
	void HandleSlotUseRequested(int32 SlotIndex, UUserWidget* Sender);

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UPanelWidget> SlotContainer;

	UPROPERTY()
	TArray<URSInventorySlotWidget*> SlotWidgets;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<URSInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(EditAnywhere, Category="Inventory|Layout")
	int32 Columns = 5;

	UPROPERTY(EditAnywhere, Category="Inventory|Layout")
	int32 Rows = 10;

	UPROPERTY(EditAnywhere, Category="Inventory|Layout")
	bool bAutoBuildSlots = true;

private:
	UPROPERTY()
	TObjectPtr<URSInventoryComponent> InventoryComp;

	FDelegateHandle InventoryChangedHandle;
};